/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
*/

#ifndef __SIGYN_ME_H
#define __SIGYN_ME_H

#include <time.h>

#include "irc.h"

struct me {
    irc_user_t *client;
    char **channels;
    struct uplink {
#ifdef _WIN32
        bool winsock;
        SOCKET sock;
#else
        int sock;
#endif
        int port;
        char *hostname;
    } uplink;
    struct stats {
        time_t start;
        int inB;
        int outB;
    } stats;
} me;

#endif
