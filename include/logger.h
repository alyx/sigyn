/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */
#ifndef __SIGYN_LOG_H
#define __SIGYN_LOG_H
//#pragma once

#include <stdio.h>
#include <stdarg.h>
#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

/*
 * define various log levels;
 * this is only for making calls to logger() look prettier.
 */
enum log_levels {
    LOG_RAW,
    LOG_DEBUG,
    LOG_WARNING,
    LOG_STATUS,
    LOG_GENERAL
};

FILE *logfile;

extern void logger_init(const char *filename);
//static void va_logger(unsigned int level, const char *format, va_list args);
extern void logger(unsigned int level, char *format, ...);

#endif
