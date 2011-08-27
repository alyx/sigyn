/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */
#ifndef __SIGYN_H
#define __SIGYN_H

#ifdef _WIN32
#   include <winsock2.h>
#   include <ws2tcpip.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#ifndef _WIN32
#   include <sys/socket.h>
#   include <netdb.h>
#   include <arpa/inet.h>
#   include <fcntl.h>
#   define close_portable close
#   define ERRNO errno
#else
#   define snprintf sprintf_s
#   define close_portable closesocket
#   define ERRNO WSAGetLastError()
#   define errno GetLastError()
#endif

#include <libmowgli/mowgli.h>
#include "atheme_string.h"
#include "irc.h"
#include "logger.h"
#include "me.h"
#include "sigyn_config.h"

extern irc_event_t *parse(char *text);
extern void sigyn_fatal(char *format, ...);
extern int StartWSA(void);
extern int sigyn_hostname(char *host, int len);
extern void uplink_connect(char *uplink, int port);
extern void uplink_disconnect(void);

#endif
