/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */
#ifndef __SIGYN_LOG_H
#define __SIGYN_LOG_H

#include "sigyn.h"

/*
 * define various log levels;
 * this is only for making calls to logger() look prettier.
 */
enum log_levels {
    LOG_RAW,
    LOG_DEBUG,
    LOG_WARNING,
    LOG_STATUS,
    LOG_GENERAL,
    LOG_FATAL
};

FILE *logfile;

extern void logger_init(const char *filename);
extern void logger_deinit(void);
extern void logger(int level, char *format, ...);

#endif
