/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 *
 * Abstract: This module implements a wrapper for (un)loading
 * and starting modules.
 *
 * These functions almost completely from Atheme
 */

#include "sigyn.h"
#include <dlfcn.h>

#if defined (_WIN32)
#   define PLATFORM_SUFFIX ".dll"
#elif defined (__HPUX__)
#   define PLATFORM_SUFFIX ".sl"
#else
#   define PLATFORM_SUFFIX ".so"
#endif

mowgli_heap_t *module_heap;
mowgli_list_t modules;

static mowgli_module_t *module_open(const char *path, char *errbuf, int errlen)
{
    char *buffer = mowgli_alloc(strlen(path) + 20);
    void *module;

    strlcpy(buffer, path, strlen(path) + 20);

    struct stat s;
    if (stat(buffer, &s) != 0)
    {
        strlcpy(errbuf, strerror(errno), errlen);
        return NULL;
    }

    module = mowgli_module_open(buffer);
    mowgli_free(buffer);

    if (!module)
    {
        strlcpy(errbuf, dlerror(), errlen);
    }
    
    return module;
}

static module_t *_load_module(const char *pathname, char *errbuf, int errlen)
{
    mowgli_node_t *node;
    module_t *module, *old_modtarget;
    module_metadata *header;
    mowgli_module_t *handle = NULL;
#if defined(HAVE_DLINFO) && !defined(__UCLIBC__)
    struct link_map *map;
#endif
    char linker_errbuf[BUFSIZE];

    handle = module_open(pathname, linker_errbuf, BUFSIZE);

    if (!handle)
    {
        snprintf(errbuf, errlen, "module_load(): error while loading %s: %s", pathname, linker_errbuf);
        return NULL;
    }

    header = (module_metadata *) mowgli_module_symbol(handle, "_metadata");

    if (header == NULL || header->sigyn != SIGYN_MAPI)
    {
        snprintf(errbuf, errlen, "module_load(): Attempted to load incompatible module %s. Aborting.", pathname);
        mowgli_module_close(handle);
        return NULL;
    }

    if (header->abi_ver != SIGYN_MAPI_VER)
    {
        snprintf(errbuf, errlen, "module_load(): Module API version mismatch (%u != %u) in %s. Aborting.", header->abi_ver, SIGYN_MAPI_VER, pathname);
        mowgli_module_close(handle);
        return NULL;
    }

    /*if (module_find_published(header->name))*/
    /*{*/
        /*snprintf(errbuf, errlen, "module_load(): %s: Module %s already exists. Aborting.", pathname, header->name);*/
        /*mowgli_module_close(handle);*/
        /*return NULL;*/
    /*}*/

    module = mowgli_heap_alloc(module_heap);

    strlcpy(module->modpath, pathname, BUFSIZE);
    strlcpy(module->name, header->name, BUFSIZE);
    module->can_unload = header->can_unload;
    module->handle = handle;
    module->mflags = MODTYPE_STANDARD;
    module->metadata = header;

#if defined(HAVE_DLINFO) && !defined(__UCLIBC__)
    dlinfo(handle, RTLD_DI_LINKMAP, &map);
    if (map != NULL)
        module->address = (void *) map->l_addr;
    else
        module->address = handle;
#else
    module->address = handle;
#endif

    if (header->modinit)
        header->modinit(module);

    logger(LOG_STATUS, "module_load(): loaded %s [at 0x%lx; MAPI version %d]", header->name, (unsigned long)module->address, header->abi_ver);

    return module;
}

void module_init(void)
{
    module_heap = mowgli_heap_create(sizeof(module_t), 256, BH_NOW);
    if (!module_heap)
       sigyn_fatal("module_init(): Mowgli block allocation failed.");
}

module_t *module_load(const char *file)
{
    module_t *module;
    char pathbuf[BUFSIZE], errbuf[BUFSIZE];
    const char *pathname;

    if (*file == '/')
        pathname = file;
    else
    {
        snprintf(pathbuf, BUFSIZE, "%s/modules/%s", SIGYN_ROOT, file);
        pathname = pathbuf;
    }
    
    /*if ((module = module_find(pathname)))*/
    /*{*/
        /*logger(LOG_STATUS, "module_load(): module %s is already loaded [at 0x%lx]", pathname, (unsigned long)m->address);*/
        /*return NULL;*/
    /*}*/

}
