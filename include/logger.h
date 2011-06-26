/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#pragma once

#include <stdio.h>
#include <stdarg.h>
#include "config.h"
#include <string.h>
#include <stdlib.h>

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

void logger(int level, char *message, ...)
{
    if ((logfile = fopen(LOGFILE, "a")) == NULL)
    {
        fprintf(stderr, "Cannot open logfile\n");
        exit(1);
    }
    if (level >= LOG_LEVEL)
    {
        char logline[1024];
        va_list args;
        va_start(args, message);
        vsnprintf(logline, 1024, message, args);
        va_end(args);
        /*strlcpy(logline, "\n");*/
        strncat(logline, "\n", 1);
        fprintf(logfile, logline);
    }
}
