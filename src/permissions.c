/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

/*
 * Routine Description:
 * This routine searches through the permissions list to locate
 * a user and check if they have a priv.
 *
 * Arguments:
 *     nickname   - nickname of questioned user
 *     permission - permission to check for
 *
 * Return value:
 *     AC_ADMIN - Returns priv if the user has the priv,
 *                and returns AC_NONE if the user does not.
 *
 */

unsigned int has_priv(const char * nickname, unsigned int perm)
{
    if ((strcmp(config_find_entry(me.config->entries, "admin")->vardata, nickname)) == 0)
        return AC_ADMIN;

    return AC_NONE;
}

