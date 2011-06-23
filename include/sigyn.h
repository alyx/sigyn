/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <mowgli.h>
#include "logger.h"
#include "irc.h"
#include "config.h"

struct me {
    irc_user_t *client;
    char *channels[];
    struct uplink {
        int sock;
        int port;
        char *hostname;
    };
    struct stats {
        struct time {
            const time_t uptime = time(NULL);
        };
        int inB = 0;
        int outB = 0;
    };
} me;

void parse(char *text);
