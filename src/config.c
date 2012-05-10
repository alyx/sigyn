/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

void config_check(mowgli_config_file_t * config)
{
    mowgli_config_file_entry_t * entry;
    
    char * nick, * desc, * server;
    uint16_t port;

    entry = config_fatal_find_entry(config->entries, "sigyn");
    nick = (entry = config_fatal_find_entry(entry->entries, "nick"))->vardata;
    desc = (entry = config_fatal_find_entry(entry, "desc"))->vardata;

    entry = config_fatal_find_entry(config->entries, "uplink");
    server = (entry = config_fatal_find_entry(entry->entries, "server"))->vardata;
    port = (uint32_t)atoi((entry = config_fatal_find_entry(entry, "port"))->vardata);

    initialise_sigyn(nick, nick, desc, server, port);
}

mowgli_config_file_entry_t * config_find_entry(mowgli_config_file_entry_t * start,
        const char * name)
{
    mowgli_config_file_entry_t * e;

    e = start;

    while(e != NULL)
    {
        if (!strcmp(e->varname, name))
            return e;
        else
            e = e->next;
    }

    return NULL;
}


mowgli_config_file_entry_t * config_fatal_find_entry(mowgli_config_file_entry_t * entries,
        char * name)
{
    mowgli_config_file_entry_t * entry;
    entry = config_find_entry(entries, name);

    if (entry == NULL)
    {
        sigyn_fatal("%s was not found in configuration file. Aborting.", name);
    }

    return entry;
}

void config_print(mowgli_config_file_entry_t * entries, int level)
{
    mowgli_config_file_entry_t * e;

    e = entries;

    while (e != NULL)
    {
        fprintf(stderr, "%d:\t%s\t%s\n", level, e->varname, e->vardata);
        if (e->entries != NULL)
            config_print(e->entries, level+1);
        e = e->next;
    }
}
