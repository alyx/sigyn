/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <libmowgli/mowgli.h>
#include "logger.h"
#include "irc.h"
#include "config.h"

struct me {
    irc_user_t *client;
    char **channels;
    struct uplink {
        int sock;
        int port;
        char *hostname;
    } uplink;
    struct stats {
        time_t start;
        int inB;
        int outB;
    } stats;
} me;

//me.stats.start = time(NULL);
//me.stats.inB = 0;
//me.stats.outB = 0;

irc_event_t *parse(char *text);
void uplink_connect(char *uplink, int *port);
