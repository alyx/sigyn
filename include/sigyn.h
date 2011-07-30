/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */
#ifndef __SIGYN_H
#define __SIGYN_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#ifdef _WIN32
#   include <winsock2.h>
#   include <w2tcpip.h>
#else
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <netdb.h>
#endif


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
        const time_t start;
        int inB;
        int outB;
    } stats;
} me;

irc_event_t *parse(char *text);
void uplink_connect(char *uplink, int *port);
#endif
