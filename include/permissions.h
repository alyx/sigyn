/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "me.h"

#ifndef SIGYN_PERMISSIONS_H
#define SIGYN_PERMISSIONS_H


#define AC_NONE  0x1
#define AC_ADMIN 0x2

//unsigned int has_priv(const char *nick, unsigned int perm);

#if 0
static inline bool has_priv(const char * nick, unsigned int priv)
#endif
static inline unsigned int has_priv(const char * nick, unsigned int priv)
{
    /* Magic goes here to find a client's privs. Just use the old admin block for now. */
#if 0
    if (u->privs & priv)
        return true;
    return false;
#endif
    if ((strcmp(config_find_entry(me.config->entries, "admin")->vardata, nick)) == 0)
        return AC_ADMIN;

    return AC_NONE;
}
#endif
