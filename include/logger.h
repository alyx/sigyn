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

void logger_init(const char *filename)
{
    if ((logfile = fopen(filename, "a")) == NULL)
    {
        fprintf(stderr, "Cannot open logfile\n");
        exit(1);
    }
}

static void va_logger(unsigned int level, const char *format, va_list args)
{
    char buf[BUFSIZE];
    char datetime[64];
    time_t t;
    struct tm tm;

    vsnprintf(buf, BUFSIZE, format, args);

    time(&t);
    tm = *localtime(&t);
    strftime(datetime, sizeof(datetime) - 1, "[%d/%m/%Y %H:%M:%S]", &tm);

    fprintf(logfile, "%s %s\n", datetime, buf);
}

void logger(unsigned int level, char *format, ...)
{
    va_list args;
    va_start(args, format);
    va_logger(level, format, args);
    va_end(args);
}
