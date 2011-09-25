#include "sigyn.h"

DECLARE_MODULE("core/help", 0, _modinit, _moddeinit,
        "1.0", "Alyx <alyx@malkier.net>");

static void cmd_help(irc_event_t *event, int parc, char **parv);

void _modinit(module_t *m)
{
    command_add("help", cmd_help, "displays information on the usage of available commands.", "<command>");
}

void _moddeinit(module_unload_intent_t intent)
{
    command_del("help", cmd_help);
}


static void cmd_help(irc_event_t *event, int parc, char **parv)
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
            config_get_string("sigyn", "fantasy"), cmd->name, cmd->syntax);
}

