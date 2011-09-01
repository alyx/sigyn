/*
 * Original code copyright (c) 2005-2007 Atheme Project (http://www.atheme.org)
 * Modifications copyright (c) 2011 Alyx Wolcott <alyx@malkier.net>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "sigyn.h"

#include <dlfcn.h>

#if defined(__APPLE__)
#   define PLATFORM_SUFFIX ".dylib"
#elif defined(__FreeBSD__) || defined(__unix__) || defined(sun)
#   define PLATFORM_SUFFIX ".so"
#elif defined(__hpux)
#   define PLATFORM_SUFFIX ".sl"
#elif defined(_MSC_VER)
#   define PLATFORM_SUFFIX ".dll"
#else
#   error Unknown platform - Dynamic library suffix unknown.
#endif

mowgli_heap_t *module_heap;
mowgli_list_t modules, modules_inprogress;

module_t *modtarget = NULL;

static module_t *module_load_internal(const char *pathname, char *errbuf, int errlen);

static mowgli_module_t *linker_open_ext(const char *path, char *errbuf, int errlen)
{
	char *buf = mowgli_alloc(strlen(path) + 20);
	void *ret;

	strlcpy(buf, path, strlen(path) + 20);

	if (!strstr(buf, PLATFORM_SUFFIX))
		strlcat(buf, PLATFORM_SUFFIX, strlen(path) + 20);

	/* Don't try to open a file that doesn't exist. */
	struct stat s;
	if (0 != stat(buf, &s))
	{
		strlcpy(errbuf, strerror(errno), errlen);
		return NULL;
	}

	ret = mowgli_module_open(buf);
	free(buf);

	if (!ret)
	{
		strlcpy(errbuf, dlerror(), errlen);
	}

	return ret;
}

void modules_init(void)
{
	module_heap = mowgli_heap_create(sizeof(module_t), 256, BH_NOW);

	if (!module_heap)
	{
		sigyn_fatal("modules_init(): block allocator failed.");
	}
}

/*
 * module_load()
 *
 * inputs:
 *       a literal filename for a module to load.
 *
 * outputs:
 *       the respective module_t object of the module.
 *
 * side effects:
 *       a module, or module-like object, is loaded and necessary initialization
 *       code is run.
 */
module_t *module_load(const char *filespec)
{
	module_t *m;
	char pathbuf[BUFSIZE], errbuf[BUFSIZE];
	const char *pathname;
	hook_module_load_t hdata;

	if (*filespec == '/')
		pathname = filespec;
	else
	{
		snprintf(pathbuf, BUFSIZE, "%s/modules/%s", SIGYN_ROOT, filespec);
		pathname = pathbuf;
	}

	if ((m = module_find(pathname)))
	{
		logger(LOG_GENERAL, "module_load(): module \2%s\2 is already loaded [at 0x%lx]", pathname, (unsigned long)m->address);
		return NULL;
	}

	m = module_load_internal(pathname, errbuf, sizeof errbuf);

	if (!m)
	{
		hdata.name = filespec;
		hdata.path = pathname;
		hdata.module = NULL;
		hdata.handled = 0;
		/*hook_call_module_load(&hdata);*/

		if (! hdata.module)
		{
			if (!hdata.handled)
				logger(LOG_WARNING, "%s", errbuf);

			return NULL;
		}

		m = hdata.module;
	}

	mowgli_node_add(m, mowgli_node_create(), &modules);

/*	if (me.connected && !cold_start)
	{
		wallops(_("Module %s loaded at 0x%lx"), m->name, (unsigned long)m->address);
		slog(LG_INFO, _("MODLOAD: \2%s\2 at 0x%lx"), m->name, (unsigned long)m->address);
	}*/

	return m;
}

/*
 * module_load_internal: the part of module_load that deals with 'real' shared
 * object modules.
 */
static module_t *module_load_internal(const char *pathname, char *errbuf, int errlen)
{
	mowgli_node_t *n;
	module_t *m, *old_modtarget;
	module_header_t *h;
	mowgli_module_t *handle = NULL;
#if defined(HAVE_DLINFO) && !defined(__UCLIBC__)
	struct link_map *map;
#endif
	char linker_errbuf[BUFSIZE];

	handle = linker_open_ext(pathname, linker_errbuf, BUFSIZE);

	if (!handle)
	{
		snprintf(errbuf, errlen, "module_load(): error while loading %s: \2%s\2", pathname, linker_errbuf);
		return NULL;
	}

	h = (module_header_t *) mowgli_module_symbol(handle, "_header");

	if (h == NULL || h->sigyn_mod != SIGYN_MAPI)
	{
		snprintf(errbuf, errlen, "module_load(): \2%s\2: Attempted to load an incompatible module. Aborting.", pathname);

		mowgli_module_close(handle);
		return NULL;
	}

	if (h->abi_ver != SIGYN_MAPI_VER)
	{
		snprintf(errbuf, errlen, "module_load(): \2%s\2: MAPI version mismatch (%u != %u), please recompile.", pathname, h->abi_ver, SIGYN_MAPI_VER);

		mowgli_module_close(handle);
		return NULL;
	}

	if (h->abi_rev != SIGYN_ABIREV)
	{
		snprintf(errbuf, errlen, "module_load(): \2%s\2: ABI revision mismatch (%u != %u), please recompile.", pathname, h->abi_rev, SIGYN_ABIREV);

		mowgli_module_close(handle);
		return NULL;
	}

	if (module_find_published(h->name))
	{
		snprintf(errbuf, errlen, "module_load(): \2%s\2: Published name \2%s\2 already exists.", pathname, h->name);

		mowgli_module_close(handle);
		return NULL;
	}

	m = mowgli_heap_alloc(module_heap);

	strlcpy(m->modpath, pathname, BUFSIZE);
	strlcpy(m->name, h->name, BUFSIZE);
	m->can_unload = h->can_unload;
	m->handle = handle;
	m->mflags = MODTYPE_STANDARD;
	m->header = h;

#if defined(HAVE_DLINFO) && !defined(__UCLIBC__)
	dlinfo(handle, RTLD_DI_LINKMAP, &map);
	if (map != NULL)
		m->address = (void *) map->l_addr;
	else
		m->address = handle;
#else
	/* best we can do here without dlinfo() --nenolod */
	m->address = handle;
#endif

	n = mowgli_node_create();
	mowgli_node_add(m, n, &modules_inprogress);

	/* set the module target for module dependencies */
	old_modtarget = modtarget;
	modtarget = m;

	if (h->modinit)
		h->modinit(m);

	/* we won't be loading symbols outside the init code */
	modtarget = old_modtarget;

	mowgli_node_delete(n, &modules_inprogress);
	mowgli_node_free(n);

	if (m->mflags & MODTYPE_FAIL)
	{
		snprintf(errbuf, errlen, "module_load(): module \2%s\2 init failed", pathname);
		module_unload(m, MODULE_UNLOAD_INTENT_PERM);
		return NULL;
	}

	logger(LOG_DEBUG, "module_load(): loaded %s [at 0x%lx; MAPI version %d]", h->name, (unsigned long)m->address, h->abi_ver);

	return m;
}

/*
 * module_load_dir()
 *
 * inputs:
 *       a directory containing modules to load.
 *
 * outputs:
 *       none
 *
 * side effects:
 *       qualifying modules are passed to module_load().
 */
void module_load_dir(const char *dirspec)
{
	DIR *module_dir = NULL;
	struct dirent *ldirent = NULL;
	char module_filename[4096];

	module_dir = opendir(dirspec);

	if (!module_dir)
	{
		logger(LOG_WARNING, "module_load_dir(): %s: %s", dirspec, strerror(errno));
		return;
	}

	while ((ldirent = readdir(module_dir)) != NULL)
	{
		if (!strstr(ldirent->d_name, ".so"))
			continue;

		snprintf(module_filename, sizeof(module_filename), "%s/%s", dirspec, ldirent->d_name);
		module_load(module_filename);
	}

	closedir(module_dir);
}

/*
 * module_load_dir_match()
 *
 * inputs:
 *       a directory containing modules to load, and a pattern to match against
 *       to determine whether or not a module qualifies for loading.
 *
 * outputs:
 *       none
 *
 * side effects:
 *       qualifying modules are passed to module_load().
 */
#if 0
void module_load_dir_match(const char *dirspec, const char *pattern)
{
	DIR *module_dir = NULL;
	struct dirent *ldirent = NULL;
	char module_filename[4096];

	module_dir = opendir(dirspec);

	if (!module_dir)
	{
		logger(LOG_WARNING, "module_load_dir(): %s: %s", dirspec, strerror(errno));
		return;
	}

	while ((ldirent = readdir(module_dir)) != NULL)
	{
		if (!strstr(ldirent->d_name, ".so") && match(pattern, ldirent->d_name))
			continue;

		snprintf(module_filename, sizeof(module_filename), "%s/%s", dirspec, ldirent->d_name);
		module_load(module_filename);
	}

	closedir(module_dir);
}
#endif
/*
 * module_unload()
 *
 * inputs:
 *       a module object to unload.
 *
 * outputs:
 *       none
 *
 * side effects:
 *       a module is unloaded and neccessary deinitalization code is run.
 */
void module_unload(module_t *m, module_unload_intent_t intent)
{
	mowgli_node_t *n, *tn;

	if (!m)
		return;

	/* unload modules which depend on us */
	while (m->dephost.head != NULL)
		module_unload((module_t *) m->dephost.head->data, intent);

	/* let modules that we depend on know that we no longer exist */
	MOWGLI_ITER_FOREACH_SAFE(n, tn, m->deplist.head)
	{
		module_t *hm = (module_t *) n->data;
		mowgli_node_t *hn = mowgli_node_find(m, &hm->dephost);

		mowgli_node_delete(hn, &hm->dephost);
		mowgli_node_free(hn);
		mowgli_node_delete(n, &m->deplist);
		mowgli_node_free(n);
	}

	n = mowgli_node_find(m, &modules);
	if (n != NULL)
	{
		logger(LOG_GENERAL, "module_unload(): unloaded \2%s\2", m->name);

		if (m->header && m->header->deinit)
			m->header->deinit(intent);
		mowgli_node_delete(n, &modules);
		mowgli_node_free(n);
	}

	/* else unloaded in embryonic state */
	if (m->handle)
	{
		mowgli_module_close(m->handle);
		mowgli_heap_free(module_heap, m);
	}
	else
	{
		/* If handle is null, unload_handler is required to be valid
		 * and we can't do anything meaningful if it isn't.
		 */
		m->unload_handler(m, intent);
	}
}

/*
 * module_locate_symbol()
 *
 * inputs:
 *       a name of a module and a symbol to look for inside it.
 *
 * outputs:
 *       the pointer to the module symbol, else NULL if not found.
 *
 * side effects:
 *       none
 */
void *module_locate_symbol(const char *modname, const char *sym)
{
	module_t *m;
	void *symptr;

	if (!(m = module_find_published(modname)))
	{
		logger(LOG_WARNING, "module_locate_symbol(): %s is not loaded.", modname);
		return NULL;
	}

	/* If this isn't a loaded .so module, we can't search for symbols in it
	 */
	if (!m->handle)
		return NULL;

	if (modtarget != NULL && !mowgli_node_find(m, &modtarget->deplist))
	{
		logger(LOG_WARNING, "module_locate_symbol(): %s added as a dependency for %s (symbol: %s)",
			m->name, modtarget->name, sym);
		mowgli_node_add(m, mowgli_node_create(), &modtarget->deplist);
		mowgli_node_add(modtarget, mowgli_node_create(), &m->dephost);
	}

	symptr = mowgli_module_symbol(m->handle, sym);

	if (symptr == NULL)
		logger(LOG_WARNING, "module_locate_symbol(): could not find symbol %s in module %s.", sym, modname);
	return symptr;
}

/*
 * module_find()
 *
 * inputs:
 *       a name of a module to locate the object for.
 *
 * outputs:
 *       the module object if the module is located, else NULL.
 *
 * side effects:
 *       none
 */
module_t *module_find(const char *name)
{
	mowgli_node_t *n;

	MOWGLI_ITER_FOREACH(n, modules.head)
	{
		module_t *m = n->data;

		if (!strcasecmp(m->modpath, name))
			return m;
	}

	return NULL;
}

/*
 * module_find_published()
 *
 * inputs:
 *       a published (in _header) name of a module to locate the object for.
 *
 * outputs:
 *       the module object if the module is located, else NULL.
 *
 * side effects:
 *       none
 */
module_t *module_find_published(const char *name)
{
	mowgli_node_t *n;

	MOWGLI_ITER_FOREACH(n, modules.head)
	{
		module_t *m = n->data;

		if (!strcasecmp(m->name, name))
			return m;
	}

	return NULL;
}

/*
 * module_request()
 *
 * inputs:
 *       a published name of a module to load.
 *
 * outputs:
 *       true: if the module was loaded, or is already present.
 *
 * side effects:
 *       a module might be loaded.
 */
bool module_request(const char *name)
{
	module_t *m;
	mowgli_node_t *n;

	if (module_find_published(name))
		return true;

	MOWGLI_ITER_FOREACH(n, modules_inprogress.head)
	{
		m = n->data;

		if (!strcasecmp(m->name, name))
		{
			logger(LOG_WARNING, "module_request(): circular dependency between modules %s and %s",
					modtarget != NULL ? modtarget->name : "?",
					m->name);
			return false;
		}
	}

	if (module_load(name) == NULL)
		return false;

	return true;
}

/* vim:cinoptions=>s,e0,n0,f0,{0,}0,^0,=s,ps,t0,c3,+s,(2s,us,)20,*30,gs,hs
 * vim:ts=8
 * vim:sw=8
 * vim:noexpandtab
 */
