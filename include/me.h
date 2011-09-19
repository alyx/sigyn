/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#ifndef __SIGYN_ME_H
#define __SIGYN_ME_H

#include <time.h>
#include <stdbool.h>

#include "irc.h"
#include "platform.h"

struct me {
    irc_user_t *client;
    char *config;
    struct uplink {
        bool connected;
#ifdef _WIN32
        bool winsock;
#endif
		socket_t sock;
        int port;
        char *hostname;
    } uplink;
    struct stats {
        time_t start;
        int inB;
        int outB;
    } stats;
    int maxfd;
} me;

#endif
