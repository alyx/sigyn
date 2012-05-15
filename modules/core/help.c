#include "sigyn.h"

DECLARE_MODULE("core/help", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Alyx <alyx@malkier.net>");

static void cmd_help(const irc_event_t *event, int parc, char **parv);

void _modinit(UNUSED module_t *m)
{
    command_add("help", cmd_help, 0, AC_NONE, "Displays information on the usage of available commands.", "<command>");
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("help", cmd_help);
}


static void cmd_help(const irc_event_t *event, int parc, char **parv)
{
    command_t *cmd;

    if (parc == 0)
    {
        command_t * c;
        mowgli_node_t * n;
        irc_notice(event->origin->nick, "*** Sigyn Help ***");
        MOWGLI_ITER_FOREACH(n, commands.head)
        {
            c = (command_t *)n->data;
            if (c->perm & AC_NONE || c->perm & has_priv(event->origin->nick, c->perm))
               irc_notice(event->origin->nick, "%s - %s", c->name, c->help);
        }
        return;
    }

    cmd = command_find(parv[1]);
    if (cmd == NULL)
    {
        irc_notice(event->origin->nick, "Command %s does not exist.", parv[1]);
        return;
    }

    irc_notice(event->origin->nick, "*** Help for %s ***", cmd->name);
    irc_notice(event->origin->nick, "%s %s", cmd->name, cmd->help);
    if (cmd->syntax != NULL)
        irc_notice(event->origin->nick, "Syntax: %s%s %s", config_find_entry(config_find_entry(me.config->entries, "sigyn")->entries, "fantasy")->vardata, cmd->name, cmd->syntax);
}


