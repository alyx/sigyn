/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 *
 * Abstract: This module implements a method of logging to a file
 */

#include "logger.h"

FILE *logfile;

void logger_init(const char *filename)
{
    if ((logfile = fopen(filename, "a")) == NULL)
    {
        fprintf(stderr, "Cannot open logfile\n");
        exit(1);
    }
}

void logger_deinit(void)
{
    if (logfile != NULL)
        fclose(logfile);
}

void logger(int level, char *format, ...)
{
    char buf[BUFSIZE], datetime[64];
    time_t t;
    struct tm tm;

    if (level < LOG_LEVEL)
        return;

    va_list args;
    va_start(args, format);
    vsnprintf(buf, BUFSIZE, format, args);
    va_end(args);

    if (logfile == NULL)
    {
        logger_init("sigyn.log");
    }

    time(&t);
    tm = *localtime(&t);
    strftime(datetime, sizeof(datetime) - 1, "[%d/%m/%Y %H:%M:%S]", &tm);
    fprintf(logfile, "%s %s\n", datetime, buf);
    fflush(logfile);
}
