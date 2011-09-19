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

/* Define some limits. */
#define BUFSIZE 1024
#define MAXPARC 20

#include "minini/minini.h"
#include "platform.h"
#include "command.h"
#include "irc.h"
#include "logger.h"
#include "me.h"
#include "module.h"
#include "queue.h"
#include "tools.h"
#include "version.h"

#endif
