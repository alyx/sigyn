/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

void config_check(mowgli_config_file_t * config)
{
    mowgli_config_file_entry_t * entry, * root;

    char * nick, * desc, * server, * vhost, * ident, * port, * ssl;
    bool use_ssl = false;

    root = config_fatal_find_entry(config->entries, "sigyn");
    nick = (config_fatal_find_entry(root->entries, "nick"))->vardata;
    desc = (config_fatal_find_entry(root->entries, "desc"))->vardata;
    entry = config_find_entry(root->entries, "ident");
    if (entry != NULL && entry->vardata != NULL)
        ident = entry->vardata;
    else
        ident = nick;

    root = config_fatal_find_entry(config->entries, "uplink");
    server = (config_fatal_find_entry(root->entries, "server"))->vardata;
    port = ((config_fatal_find_entry(root->entries, "port"))->vardata);
    if ((entry = config_find_entry(root->entries, "ssl")))
        if ((int)strtol(entry->vardata, (char **)NULL, 10) == 1)
            use_ssl = true;
    if ((entry = config_find_entry(root->entries, "vhost")))
        vhost = entry->vardata;

    initialise_sigyn(nick, ident, desc, server, port, use_ssl, vhost);
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
