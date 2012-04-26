/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 *
 * Abstract: This module implements a method of logging to a file
 */

#include "sigyn.h"

static mowgli_heap_t * logheap;
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
    newlog->isFile  = true;

    mowgli_node_add(newlog, mowgli_node_create(), &loglocs); 

    return 0;
}

int logger_add_channel(const char * channel, unsigned int level)
{
    logger_t * newlog;

    newlog = mowgli_heap_alloc(logheap);

    newlog->channel = strdup(channel);
    newlog->level   = level;
    newlog->isFile    = false;

    mowgli_node_add(newlog, mowgli_node_create(), &loglocs);

    return 0;
}

/*
 * Routine Description:
 * This routine initializes the logger by opening the specified file
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
    FILE * out;
    logger_t * l;
    mowgli_config_file_entry_t * e, * f, * g;
    e = config;

    logheap = mowgli_heap_create(sizeof(logger_t), 1024, BH_NOW);
    if (!logheap)
        sigyn_fatal("logger_init(): block allocator failed.");

    while (e != NULL)
    {
        if (!strcmp(e->varname, "log") && e->entries != NULL)
        {
            l = mowgli_heap_alloc(logheap);
            /*f = e->entries;*/
            f = config_find_entry(e->entries, "type");
            if (f == NULL)
                mowgli_heap_free(logheap, l);

            if (!strcmp("file", f->vardata))
                l->isFile = true;
            else if (!strcmp("channel", f->vardata))
                l->isFile = false;
            else
                mowgli_heap_free(logheap, l);

            f = config_find_entry(e->entries, "level");
            if (f == NULL)
                mowgli_heap_free(logheap, l);
            if (f->entries == NULL)
                mowgli_heap_free(logheap, l);
            g = f->entries;
            l->level = 0;
            while (g != NULL)
            {
                if (!strcmp(g->varname, "raw"))
                    l->level |= LOG_RAW;
                else if (!strcmp(g->varname, "debug"))
                    l->level |= LOG_DEBUG;
                else if (!strcmp(g->varname, "error"))
                    l->level |= LOG_ERROR;
                else if (!strcmp(g->varname, "general"))
                    l->level |= LOG_GENERAL;
                else if (!strcmp(g->varname, "all"))
                    l->level |= LOG_ALL;

                g = g->next;
            }

            f = config_find_entry(e->entries, "location");
            if (f == NULL || f->vardata == NULL)
                mowgli_heap_free(logheap, l);
            if (l->isFile == true)
                l->f = fopen(f->vardata, "a");
            else
                l->channel = strdup(f->vardata);

            mowgli_node_add(l, mowgli_node_create(), &loglocs);
        }
        e = e->next;
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

        if (l->isFile)
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
            if (l->isFile)
            {
		fprintf(l->f, "%s %s\n", datetime, strip_colors_codes(buf));
                fflush(l->f);
            }
            else
                irc_privmsg(l->channel, "%s %s", datetime, buf);
        }
    }
}
