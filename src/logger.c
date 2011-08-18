/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 *
 * Abstract: This module implements a method of logging to a file
 */

#include "logger.h"

/*
 * Routine Description:
 * This routine initialises the logger by opening the specified file
 * and setting it as Sigyn's logfile.
 *
 * Arguments:
 *     filename - A string containing the path of the log file.
 *
 * Return value:
 *     None
 */

void logger_init(const char *filename)
{
    if ((logfile = fopen(filename, "a")) == NULL)
    {
        fprintf(stderr, "Cannot open logfile\n");
        exit(1);
    }
}

/*
 * Routine Description:
 * This routine deinitialises the logger by closing the log file.
 *
 * Arguments:
 *     None
 *
 * Return value:
 *     None
 */

void logger_deinit(void)
{
    if (logfile != NULL)
        fclose(logfile);
}

/*
 * Routine Description:
 * This routine provides a method for logging various details to a file.
 *
 * Arguments:
 *     level  - An integer specifying the level to log the message to (See
 *              the log_levels enum in logger.h for the log levels).
 *     format - A string containing the format for the log message.
 *     ...    - An undefined amount of other arguments to be inserted
 *              into the format string.
 *
 * Return value:
 *     None
 */

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
