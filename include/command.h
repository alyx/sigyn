/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#ifndef __SIGYN_COMMAND_H
#define __SIGYN_COMMAND_H

#include "mowgli.h"

typedef struct command_ command_t;
typedef void (*command_function_t)(const void *event, int user_argstack_size, char **user_arguments);

struct command_
{
    char *name;
    command_function_t function;
    char *help;
    char *syntax;
};

extern void command_init(void);
extern command_t *command_find(const char *name);
extern void command_add(const char *name, void *function, const char *help, const char *syntax);
extern void command_del(const char *name, void *function);


#endif /* __SIGYN_COMMAND_H */
