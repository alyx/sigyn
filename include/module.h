#ifndef SIGYN_MODULE_H
#define SIGYN_MODULE_H

#include "config.h"

typedef enum {
    MODULE_UNLOAD_INTENT_PERN,
    MODULE_UNLOAD_INTENT_RELOAD
} module_unload_intent_t;

typedef enum {
    MODULE_UNLOAD_OK,
    MODULE_UNLOAD_NEVER,
    MODULE_UNLOAD_RELOAD_ONLY
} module_unload_capability_t;

typedef struct module_t module_t;
typedef struct module_metadata module_metadata;
typedef void (*module_unload_handler_t)(module_t *, module_unload_intent_t);

struct module_t {
    char name[BUFSIZE];
    char modpath[BUFSIZE];
    module_unload_capability_t can_unload;

    unsigned int mflags;

    module_metadata *metadata;

    void *address;
    mowgli_module_t *handle;

    module_unload_handler_t unload_handler;

    mowgli_list_t dephost;
    mowgli_list_t deplist;

    mowgli_list_t symlist;
};

#define MODTYPE_STANDARD    0
#define MODTYPE_CORE        1 /* Cannot be unloaded. */

#define SIGYN_MAPI          0xDEADF00D
#define SIGYN_MAPI_VER      1
#define SIGYN_ABI_REV       100000

struct module_metadata {
    unsigned int sigyn;
    unsigned int abi_ver;
    unsigned int abi_rev;
    const char *serial;
    const char *name;
    module_unload_capability_t can_unload;
    void (*modinit)(module_t *module);
    void (*deinit)(module_unload_intent_t intent);
    const char *vendor;
    const char *version;
};

#define DECLARE_MODULE(name, norestart, modinit, deinit, version, vendor) \
    module_metadata _metadata = { \
        SIGYN_MAPI, SIGYN_MAPI_VER, \
        SIGYN_ABI_REV, "unknown", \
        name, norestart, modinit, deinit, vendor, version \
    }

extern void _modinit(module_t *module);
extern void _moddeinit(module_unload_intent_t intent);

extern void modules_init(void);
extern module_t *module_load(const char *file);


#endif
