/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

mowgli_heap_t *command_heap;
mowgli_list_t commands;

static void handle_privmsg(void *data, UNUSED void *udata);

/*
 * Routine Description:
 * This routine initialises the command memory heap.
 *
 * Arguments:
 *     None
 *
 * Return value:
 *      None
 *
 */

void command_init(void)
{
    command_heap = mowgli_heap_create(sizeof(command_t), 1024, BH_NOW);

    if (!command_heap)
        sigyn_fatal("command_init(): block allocator failed.");

    mowgli_hook_associate("PRIVMSG", handle_privmsg, NULL);
}

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

command_t *command_find(const char *name)
{
    command_t *c;
    mowgli_node_t *n;

    MOWGLI_ITER_FOREACH(n, commands.head)
    {
        c = (command_t *)n->data;

        if (!strcasecmp(c->name, name))
            return c;
    }

    return NULL;
}

/*
 * Routine Description:
 * This routine provides a function to create a command and append it to
 * the command list.
 *
 * Arguments:
 *     name     - A string containing the name of the command.
 *     function - A pointer to the function to be called when
 *                the command is used.
 *     help     - A string containing information about the command.
 *     syntax   - A string containing the command syntax.
 * 
 * Return value:
 *     None
 *
 */

void command_add(const char *name, void *function, const char *help, const char *syntax)
{
    command_t *c;

    if ((command_find(name)) != NULL)
    {
        logger(LOG_DEBUG, "Command %s already exists. Aborting command_add().", name);
        return;
    }

    c = mowgli_heap_alloc(command_heap);

    c->name = strdup(name);
    c->function = (command_function_t)function;
    c->help = strdup(help);
    c->syntax = strdup(syntax);

    mowgli_node_add(c, mowgli_node_create(), &commands);
}

/*
 * Routine Description:
 * This routine provides a function to remove a command from the command list.
 *
 * Arguments:
 *     name     - A string containing the name of the command.
 *     function - A pointer to the function the command pointed to.
 *
 * Return value:
 *     None
 *
 */

void command_del(const char *name, void *function)
{
    command_t *c;
    mowgli_node_t *n, *tn;

    MOWGLI_LIST_FOREACH_SAFE(n, tn, commands.head)
    {
        c = (command_t *)n->data;
        if ((c->function == (command_function_t)function) && ((strcmp(c->name, name)) == 0))
        {
            mowgli_free(c->name);
            mowgli_free(c->help);
            mowgli_free(c->syntax);

            mowgli_node_delete(n, &commands);
            mowgli_heap_free(command_heap, c);
        }
    }
}

static void handle_privmsg(void *data, UNUSED void *udata)
{
    int i, len;
    irc_event_t *event;
    const irc_event_t *clone;
    command_t *cmd;
    int parc;
    char *parv[MAXPARC + 1], *tmp, *prefix;

    event = (irc_event_t *)data;
    clone = event;

    tmp = strdup(event->data);
    prefix = config_get_string("sigyn", "fantasy");
    if (prefix == NULL)
        return;

    len = (strlen(prefix) - 1);

    for (i = 0; i <= len; i++)
    {
        if (*tmp == prefix[i])
        {
            parc = tokenize(tmp, parv);

            cmd = command_find(parv[0] + 1);
            if (cmd != NULL)
                cmd->function(clone, parc, parv);
            break;
        }
    }

    free(tmp);
}
