#include "sigyn.h"

DECLARE_MODULE("admin/join", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Justin Crawford <Justasic@gmail.com>");

static void cmd_join(const irc_event_t *event, int parc, char **parv);

void _modinit(UNUSED module_t *m)
{
    command_add("join", cmd_join, "Joins an IRC channel", "<channel> [<key>]");
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("join", cmd_join);
}


static void cmd_join(const irc_event_t *event, int parc, char **parv)
{
    if ((strcmp(config_find_entry(me.config->entries, "admin")->vardata, event->origin->nick)) != 0)
      return;

    if (parc < 1)
    {
        command_fail(CMD_NEEDSPARAM, event->origin, "join");
        return;
    }

    irc_join(parv[1], parc > 2?parv[2]:NULL);
    irc_notice(event->origin->nick, "Joined \2%s\2 successfully", parv[1]);
}