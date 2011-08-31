/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 *
 * Abstract: This module implements a wrapper for (un)loading
 * and starting modules.
 *
 * Much code borrowed from Atheme
 */

 #include "sigyn.h"
 #include <dlfcn.h>

 mowgli_heap_t *module_heap;
 mowgli_list_t modules;

 void sigyn_module_init(void)
 {
     module_heap = mowgli_heap_create(sizeof(module_t));
     if (!module_heap)
        sigyn_fatal("sigyn_module_init(): Mowgli block allocation failed.")
 }

module_t *sigyn_module_load(const char *file)
{
    module_t *module;
    char pathbuf[BUFSIZE], errbuf[BUFSIZE];
    const char *pathname;

}
