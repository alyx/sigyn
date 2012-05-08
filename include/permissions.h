/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#ifndef SIGYN_PERMISSIONS_H
#define SIGYN_PERMISSIONS_H

#define AC_NONE  0x1
#define AC_ADMIN 0x2

unsigned int has_priv(const char *nick, unsigned int perm);

#endif
