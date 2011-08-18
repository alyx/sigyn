/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */
#ifndef __SIGYN_H
#define __SIGYN_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <libmowgli/mowgli.h>
#include "me.h"
#include "platform.h"
#include "logger.h"
#include "irc.h"
#include "sigyn_config.h"

extern irc_event_t *parse(char *text);
extern void sigyn_fatal(char *format, ...);
extern int StartWSA(void);
extern int sigyn_hostname(char *host, int len);
extern void uplink_connect(char *uplink, int port);
extern void uplink_disconnect(void);

#endif
