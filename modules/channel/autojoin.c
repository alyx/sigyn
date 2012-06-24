#include "sigyn.h"
#include <stdio.h>

DECLARE_MODULE("channel/autojoin", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "bikcmp <bikcmp@irondust.net>");

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
    /* autojoin */
    mowgli_config_file_entry_t * entry;
    char * entrytok;
    char * entrydup;
    entry = config_find_entry(me.config->entries, "autojoin");
    if (entry && entry->vardata)
    {
		entrydup=mowgli_strdup(entry->vardata);
        entrytok = strtok(entrydup,",");
        while (entrytok != NULL)
        {
            irc_join(entrytok,NULL);
            entrytok = strtok(NULL,",");
        }
    }
    /* joinlogchan */
    entry = config_find_entry(me.config->entries, "joinlogchan");
    if (entry && entry->vardata)
    {
		entrydup=mowgli_strdup(entry->vardata);
		entrytok = strtok(entrydup,",");
        while (entrytok != NULL)
        {
            irc_join(entrytok,NULL);
            entrytok = strtok(NULL,",");
        }
    }
}
