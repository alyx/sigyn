#include "sigyn.h"

DECLARE_MODULE("channel/autojoin", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Alyx <alyx@malkier.net>");

static void handle_001(void * data, UNUSED void * udata);

void _modinit(UNUSED module_t *m)
{
    mowgli_hook_associate("001", handle_001, NULL);
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    mowgli_hook_dissociate("001", handle_001);
}

void handle_001(void * data, UNUSED void * udata)
{
    mowgli_config_file_entry_t * entry;
    mowgli_node_t * n;
    logger_t * loc;
    char * buf;
    size_t size;

    buf = mowgli_alloc(BUFSIZE);
    size = BUFSIZE - 1;
    entry = config_find_entry(me.config->entries, "autojoin");
    if (entry != NULL)
    {
        if (entry->entries != NULL)
        {
            entry = entry->entries;
            while (entry != NULL)
            {
                mowgli_strlcat(buf, entry->varname, size);
                mowgli_strlcat(buf, ",", size);
                size = size - (strlen(entry->varname) + 1);
                entry = entry->next;
            }
        }
    }

    entry = config_find_entry(me.config->entries, "joinlogchan");
    if (entry)
    {
        MOWGLI_ITER_FOREACH(n, loglocs.head)
        {
            loc = (logger_t *)n->data;

            if (!loc->isFile && loc->channel != NULL)
            {
                mowgli_strlcat(buf, loc->channel, size);
                mowgli_strlcat(buf, ",", size);
                size = size - (strlen(loc->channel) + 1);
            }
        }
    }

    irc_join(buf, NULL);
}
