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

typedef enum log_levels log_level_t;
typedef struct _logger logger_t;

struct _logger
{
    union {
        FILE * f;
        char * channel;
    }

    log_level_t level;
    bool file;
}

extern logger_t * logger_add_file(const char * file, log_level_t level);
extern void logger_init(const char *filename);
extern void logger_deinit(void);
extern void logger(log_level_t level, char *format, ...);

#endif
