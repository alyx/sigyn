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

#include <libmowgli/mowgli.h>
#include "logger.h"
#include "irc.h"
#include "config.h"

struct me {
    irc_user_t *client;
    char **channels;
    struct uplink {
#ifdef _WIN32
        SOCKET sock;
#else
        int sock;
#endif
        int port;
        char *hostname;
    } uplink;
    struct stats {
        const time_t start;
        int inB;
        int outB;
    } stats;
} me;

extern irc_event_t *parse(char *text);
extern void uplink_connect(char *uplink, int *port);
extern char *sigyn_hostname(void);
extern void sigyn_fatal(void);
#endif
