/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#pragma once

#include "sigyn.h"

static void nick(char *newnick)
{
    raw("NICK %s", newnick);
}

static void user(char *user, char *host, char *server, char *real)
{
    raw("USER %s %s %s :%s", user, host, server, real);
}

static void quit(char *message)
{
    raw("QUIT :%s", message);
}

static void pong(char *message)
{
    raw("PONG :%s", message);
}
