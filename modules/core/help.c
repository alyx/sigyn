#include "sigyn.h"

DECLARE_MODULE("core/help", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Alyx <alyx@malkier.net>");

static void cmd_help(const irc_event_t *event, int parc, char **parv);

void _modinit(UNUSED module_t *m)
{
    command_add("help", cmd_help, "displays information on the usage of available commands.", "<command>");
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("help", cmd_help);
}


static void cmd_help(const irc_event_t *event, int parc, char **parv)
{
    command_t *cmd;

    if (parc < 1)
    {
        command_fail(CMD_NEEDSPARAM, event->origin, "help");
        return;
    }

    cmd = command_find(parv[1]);
    if (cmd == NULL)
    {
        irc_notice(event->origin->nick, "Command %s does not exist.");
        return;
    }

    irc_notice(event->origin->nick, "*** Help for %s ***", cmd->name);
    irc_notice(event->origin->nick, "%s %s", cmd->name, cmd->help);
    irc_notice(event->origin->nick, "Syntax: %s%s %s",
            config_find_entry(config_find_entry(me.config->entries, "sigyn")->entries, "fantasy")->vardata, cmd->name, cmd->syntax);
}


