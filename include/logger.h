/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include <stdio.h>
#include <stdarg.h>
#include "config.h"

/*
 * define various log levels;
 * this is only for making calls to log() look prettier.
 */
enum log_levels {
    LOG_RAW,
    LOG_DEBUG,
    LOG_WARNING,
    LOG_STATUS,
    LOG_GENERAL
}

void log(int level, char *message, ...)
{
    if (level >= LOG_LEVEL)
    {
        char logline[1024];
        va_list args;
        va_start(args, message);
        vsnprinft(logline, 1024, message, args);
        va_end(args);
        strlcpy(logline, "\n");
        fprintf(logfile, logline);
    }
}
