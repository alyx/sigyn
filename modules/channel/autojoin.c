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
    mowgli_config_file_entry_t *autojoin_entry;
    mowgli_config_file_entry_t *logchan_entry;

    /**
     * IRC supports JOIN #chan1,#chan2 so instead of 
     * splitting these by comma and sending 1 JOIN per
     * element, just send as-is. Shouldn't be a problem
     * for most modern ircds.
     **/

    autojoin_entry = config_find_entry(me.config->entries, "autojoin");
    if(autojoin_entry)
        irc_join(autojoin_entry->vardata, NULL);

    logchan_entry = config_find_entry(me.config->entries, "joinlogchan");
    if(logchan_entry)
        irc_join(logchan_entry->vardata, NULL);

}