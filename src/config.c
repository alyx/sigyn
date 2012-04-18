#include "sigyn.h"

void config_check(mowgli_config_file_t * config)
{
    mowgli_config_file_entry_t * entry;
    
    char * nick, * desc, * server, fantasy;
    uint16_t port;

    entry = config_fatal_find_entry(config->entries, "sigyn");
    nick = (entry = config_fatal_find_entry(entry->entries, "nick"))->vardata;
    desc = (entry = config_fatal_find_entry(entry, "desc"))->vardata;
    fantasy = (char)(entry = config_fatal_find_entry(entry, "fantasy"))->vardata;

    entry = config_fatal_find_entry(config->entries, "uplink");
    server = (entry = config_fatal_find_entry(entry->entries, "server"))->vardata;
    port = (uint32_t)atoi((entry = config_fatal_find_entry(entry, "port"))->vardata);

    initialise_sigyn(nick, nick, desc, server, port);

}

#if 0
mowgli_config_file_entry_t * config_find_entry(mowgli_config_file_entry_t *entries, 
        char * name)
{
    mowgli_config_file_entry_t * entry, * subentry;

    while(entry != NULL)
    {
        if (!strcmp(entry->varname, name))
            return entry;
        else
            entry = entry->next;
    }

    return NULL;
}
#endif

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
        printf("LOL FAIL! %s\n", name);
        exit(-1);
    }

    return entry;
}
