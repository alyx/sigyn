/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */
#ifndef __SIGYN_H
#define __SIGYN_H

#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <libmowgli/mowgli.h>

#define HOSTLEN 64
#include "platform.h"
#include "irc.h"
#include "ircstream.h"
#include "logger.h"
#include "me.h"
#include "module.h"
#include "queue.h"
#include "sigyn_config.h"

/* Define functions from string.c */

extern void strip(char *line, char *strippers); 
#ifndef HAVE_STRLCAT
extern size_t strlcat(char *dest, const char *src, size_t count);
#endif
#ifndef HAVE_STRLCPY
extern size_t strlcpy(char *dest, const char *src, size_t count);
#endif

extern irc_event_t *parse(char line[]);
extern void preparse(char line[]);
extern void sigyn_fatal(char *format, ...);
void sigyn_cleanup(void);
extern int StartWSA(void);
extern int sigyn_hostname(char *host, int len);
extern socket_t uplink_connect(char *uplink, int port, char *vhost);
extern void uplink_disconnect(void);

#endif
