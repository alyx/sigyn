/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#ifndef SIGYN_INTERFACE_H
#define SIGYN_INTERFACE_H

#include "irc.h"

enum command_failure_reasons {
    CMD_BADPARAM,
    CMD_NEEDSPARAM,
    CMD_NOAUTH
};

extern void command_fail(const int reason,
                         const irc_user_t *origin,
                         const char *command);

extern void command_reply(const char * target, char * fmt, ...);
extern void command_reply_safe(const char * target, char * fmt, ...);

#endif /* SIGYN_INTERFACE_H */
