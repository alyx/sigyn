/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "irc.h"

enum command_failure_reasons {
    CMD_BADPARAM,
    CMD_NEEDSPARAM,
    CMD_NOAUTH
};

extern void command_fail(int reason, irc_user_t *origin, const char *command);
