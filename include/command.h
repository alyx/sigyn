/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
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
//extern command_t *command_find(const char *name);
extern void command_add(const char *name, void *function, const char *help, const char *syntax);
extern void command_del(const char *name, void *function);

/*
 * Routine Description:
 * This routine searches through the command list to locate
 * the specified command.
 *
 * Arguments:
 *     name - A string containing the name of the command to seach for.
 *
 * Return value:
 *     c - Returns a command_t object if the command is located,
 *         and returns NULL if it is not.
 *
 */

static inline command_t * command_find(const char * name)
{
    command_t * c;
    mowgli_node_t * n;

    MOWGLI_ITER_FOREACH(n, commands.head)
    {
        c = (command_t *)n->data;

        if (!strcasecmp(c->name, name))
            return c;
    }

    return NULL;
}

#endif /* __SIGYN_COMMAND_H */
