/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 *
 * Abstract: This module implements a method of logging to a file
 */

#include "sigyn.h"

mowgli_heap_t * logheap;
mowgli_list_t loglocs;

int logger_add_file(const char * file, unsigned int level)
{
    FILE * f;
    logger_t * newlog;

    newlog = mowgli_heap_alloc(logheap);

    if ((f = fopen(file, "a")) == NULL)
        return -1;

    newlog->f     = f;
    newlog->level = level;
    newlog->file  = true;

    mowgli_node_add(newlog, mowgli_node_create(), &loglocs); 

    return 0;
}

int logger_add_channel(const char * channel, unsigned int level)
{
    logger_t * newlog;

    newlog = mowgli_heap_alloc(logheap);

    newlog->channel = strdup(channel);
    newlog->level   = level;
    newlog->file    = false;

    mowgli_node_add(newlog, mowgli_node_create(), &loglocs);

    return 0;
}

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

void logger_init(mowgli_config_file_entry_t * config)
{
    logger_t * l;
    mowgli_config_file_entry_t * e, f, g;
    e = config;

    logheap = mowgli_heap_create(sizeof(logger_t), 1024, BH_NOW);
    if (!logheap)
        sigyn_fatal("logger_init(): block allocator failed.");

    while (e != NULL)
    {
        if (!strcmp(e->varname, "log") && e->entries != NULL)
        {
            /*f = e->entries;*/
            f = config_find_entry(e->entries, ); 
        }
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
    logger_t * l;
    mowgli_node_t * n;

    MOWGLI_ITER_FOREACH(n, loglocs.head)
    {
        l = (logger_t *)n->data;

        if (l->file)
            fclose(l->f);
        else
            mowgli_free(l->channel);

        mowgli_node_delete(n, &loglocs);
        mowgli_heap_free(logheap, l);
    }
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

void logger(unsigned int level, char *format, ...)
{
    int i;
    char buf[BUFSIZE], datetime[64];
    logger_t * l;
    mowgli_node_t * n;
    time_t t;
    struct tm tm;

    va_list args;
    va_start(args, format);
    vsnprintf(buf, BUFSIZE, format, args);
    va_end(args);

    time(&t);
    tm = *localtime(&t);
    strftime(datetime, sizeof(datetime) - 1, "[%d/%m/%Y %H:%M:%S]", &tm);

    MOWGLI_ITER_FOREACH(n, loglocs.head)
    {
        l = (logger_t *)n->data;

        if (l->level & level)
        {
            if (l->file)
            {
                fprintf(l->file, "%s %s\n", datetime, buf);
                fflush(l->file);
            }
            else
                irc_privmsg(l->channel, "%s %s", datetime, buf);
        }
    }
}
