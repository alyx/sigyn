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
#include "config.h"

struct me {
    char *nick = SIGYA_NICK;
    char *channels[];
    struct uplink {
        int sock;
        int port = UPLINK_PORT;
        char *hostname = UPLINK_SERVER;
        struct addrinfo hints {
            int ai_family = AF_UNSPEC;
            int ai_socktype = SOCK_STREAM;
        }
        struct addrinfo *res;
    }
} me;

int raw(char *line, ...) {
    char sendbuf[510];
    va_list args;
    
    va_start(args, line);
    vsnprintf(sendbuf, 509, line, args);
    va_end(args);

    strlcpy(sendbuf, "\r\n");

    int sent = send(socket, sendbuf, strlen(sendbuf), 0);
    char logout = snprintf("<< %s", sendbuf);
    log(LOG_RAW, logout);
    return sent;
}
