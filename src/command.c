#include "sigyn.h"

mowgli_heap_t *command_heap;
mowgli_list_t commands;

static void handle_privmsg(void *data, void *unused);

void command_init(void)
{
    command_heap = mowgli_heap_create(sizeof(command_t), 1024, BH_NOW);

    if (!command_heap)
        sigyn_fatal("command_init(): block allocator failed.");

    mowgli_hook_associate("PRIVMSG", handle_privmsg, NULL);
}

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

void command_add(const char *name, void *function)
{
    command_t *c;

    if ((command_find(name)) != NULL)
    {
        logger(LOG_DEBUG, "Command %s already exists. Aborting command_add().", name);
        return;
    }
    
    c = mowgli_heap_alloc(command_heap);

    c->name = strdup(name);
    c->function = function;

    mowgli_node_add(c, mowgli_node_create(), &commands);
}

void command_del(const char *name, void *function)
{
    command_t *c;
    mowgli_node_t *n, *tn;

    MOWGLI_LIST_FOREACH_SAFE(n, tn, commands.head)
    {
        c = (command_t *)n->data;
        if ((c->function == function) && ((strcmp(c->name, name)) == 0))
        {
            mowgli_node_delete(n, &commands);
            mowgli_heap_free(command_heap, c);
        }
    }
}


static void handle_privmsg(void *data, void *unused)
{
    irc_event_t *event;
    command_t *cmd;
    int parc;
    char *parv[MAXPARC + 1], *tmp;
    
    event = (irc_event_t *)data;
    tmp = strdup(event->data);
    if (*tmp == '!')
    {
        parc = tokenize(tmp, parv);

        cmd = command_find(parv[0] + 1);
        if (cmd != NULL)
            cmd->function(event, parc, parv);
    }
    free(tmp);
}

