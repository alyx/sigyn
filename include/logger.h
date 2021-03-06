/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */
#ifndef SIGYN_LOG_H
#define SIGYN_LOG_H

#include "sigyn.h"

/*
 * define various log levels;
 * this is only for making calls to logger() look prettier.
 */
#define LOG_ALL       0x01
#define LOG_RAW       0x02
#define LOG_DEBUG     0x04
#define LOG_CRITICAL  0x08
#define LOG_ERROR     0x10
#define LOG_WARNING   0x20
#define LOG_GENERAL   0x40

typedef struct _logger logger_t;

struct _logger
{
    union {
        FILE * f;
        char * channel;
    };
    unsigned int level;
    bool isFile;
};

extern bool logger_add_file(const char * file, unsigned int level);
extern void logger_add_channel(const char * channel, const unsigned int level);
extern void logger_init(mowgli_config_file_entry_t * config);
extern void logger_deinit(void);
extern void logger(unsigned int level, char *format, ...);
extern bool should_log(unsigned int level);

mowgli_list_t loglocs;

#endif /* SIGYN_LOG_H */
