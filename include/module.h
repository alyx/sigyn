/*
 * Copyright (c) 2005 William Pitcock, et al.
 * The rights to this code are as documented in doc/LICENSE.
 *
 * This file contains data structures concerning modules.
 *
 */

#ifndef SIGYN_MODULE_H
#define SIGYN_MODULE_H

#include "abirev.h"

typedef enum {
	MODULE_UNLOAD_INTENT_PERM,
	MODULE_UNLOAD_INTENT_RELOAD,
} module_unload_intent_t;

typedef enum {
	MODULE_UNLOAD_CAPABILITY_OK,
	MODULE_UNLOAD_CAPABILITY_NEVER,
	MODULE_UNLOAD_CAPABILITY_RELOAD_ONLY,
} module_unload_capability_t;

typedef struct module_ module_t;
typedef struct module_header_t module_header_t;

typedef void (*module_unload_handler_t)(module_t *, module_unload_intent_t);

/* Module structure. Might be a loaded .so module, or something else that
 * behaves as a module for dependency purposes (perl script, etc).
 */
struct module_ {
	char name[BUFSIZE];
	char modpath[BUFSIZE];
	module_unload_capability_t can_unload;

	unsigned int mflags;

	/* These three are real-module-specific. Either all will be set, or all
	 * will be null.
	 */
	module_header_t *header;
	void *address;
	mowgli_module_t *handle;

	/* If this module is not a loaded .so (the above three are null), and
	 * can_unload is not never, then * this must be set to a working unload
	 * function.
	 */
	module_unload_handler_t unload_handler;

	mowgli_list_t dephost;
	mowgli_list_t deplist;

	mowgli_list_t symlist;		/* MAPIv2 symbol dependencies. */
};

#define MODTYPE_STANDARD	0
#define MODTYPE_CORE		1 /* Can't be unloaded. */
#define MODTYPE_FAIL		0x8000 /* modinit failed */

#define SIGYN_MAPI      	0xBEEFF00D
#define SIGYN_MAPI_VER		1

#define MAX_CMD_PARC		20

struct module_header_t {
	unsigned int sigyn_mod;
	unsigned int abi_ver;
	unsigned int abi_rev;
	const char *serial;
	const char *name;
	module_unload_capability_t can_unload;
	void (*modinit)(module_t *m);
	void (*deinit)(module_unload_intent_t intent);
	const char *vendor;
	const char *version;
};

/* name is the module name we're searching for.
 * path is the likely full path name, which may be ignored.
 * If it is found, set module to the loaded module_t pointer
 */
typedef struct {
	const char *name;
	const char *path;
	module_t *module;
	int handled;
} hook_module_load_t;

#define DECLARE_MODULE(name, norestart, modinit, deinit, ver, ven) \
	module_header_t _header = { \
		SIGYN_MAPI, SIGYN_MAPI_VER, \
		SIGYN_ABIREV, "unknown", \
		name, norestart, modinit, deinit, ven, ver \
	}

extern void _modinit(module_t *m);
extern void _moddeinit(module_unload_intent_t intent);

extern void modules_init(void);
extern module_t *module_load(const char *filespec);
extern void module_load_dir(const char *dirspec);
extern void module_load_dir_match(const char *dirspec, const char *pattern);
extern void *module_locate_symbol(const char *modname, const char *sym);
extern void module_unload(module_t *m, module_unload_intent_t intent);
extern module_t *module_find(const char *name);
extern module_t *module_find_published(const char *name);
extern bool module_request(const char *name);

#define MODULE_TRY_REQUEST_DEPENDENCY(self, modname) \
	if (module_request(modname) == false)				\
	{								\
		(self)->mflags = MODTYPE_FAIL;				\
		return;							\
	}

#define MODULE_TRY_REQUEST_SYMBOL(self, dest, modname, sym) \
	if ((dest = module_locate_symbol(modname, sym)) == NULL)		\
	{									\
		MODULE_TRY_REQUEST_DEPENDENCY(self, modname);			\
		if ((dest = module_locate_symbol(modname, sym)) == NULL)	\
		{								\
			(self)->mflags = MODTYPE_FAIL;				\
			return;							\
		}								\
	}

typedef struct module_dependency_ {
	char *name;
	module_unload_capability_t can_unload;
} module_dependency_t;

#endif

/* vim:cinoptions=>s,e0,n0,f0,{0,}0,^0,=s,ps,t0,c3,+s,(2s,us,)20,*30,gs,hs
 * vim:ts=8
 * vim:sw=8
 * vim:noexpandtab
 */
