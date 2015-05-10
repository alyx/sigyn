#include "sigyn.h"

static void cmd_join(const irc_event_t *event, int parc, char **parv);
static void cmd_part(const irc_event_t *event, int parc, char **parv);

DECLARE_MODULE("admin/join",
               MODULE_UNLOAD_CAPABILITY_OK,
               _modinit,
               _moddeinit,
               "1.0",
               "Justin Crawford <Justasic@gmail.com>");

void
_modinit(UNUSED module_t *m)
{
    command_add("join",
                cmd_join,
                1,
                AC_ADMIN,
                "Joins an IRC channel",
                "<channel> [<key>]");

    command_add("part",
                cmd_part,
                0,
                AC_ADMIN,
                "Parts an IRC channel",
                "<channel> [<reason>]");

    // TODO: Add CYCLE
}

void
_moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("join", cmd_join);
    command_del("part", cmd_part);
}

static void
cmd_part(const irc_event_t *event, int parc, char **parv)
{

    if(!parc)
    {   
        if(!channel_find(event->target))
        {
            irc_notice(event->origin->nick, "Use PART without parameters in a channel to part contextually.");
            return;
        }
        irc_part(event->target, "Part requested by \2%s\2.", event->origin->nick);
        return;
    }

    char *message = ((parc > 2) ? parv[2] : NULL);

    if(!channel_find(parv[1]))
    {
	    irc_notice(event->origin->nick, "Not currently in \2%s\2.", parv[1]);
	    return;
    }

    if (irc_part(parv[1], message))
    {
        irc_notice(event->origin->nick, "Parted \2%s\2 successfully.", parv[1]);
        return;
    }

    return;
}

static void
cmd_join(const irc_event_t *event, int parc, char **parv)
{
    char *channelkey = ((parc > 2) ? parv[2] : NULL);
    if(!ischannel(parv[1]))
    {
	    irc_notice(event->origin->nick, "Cannot join \2%s\2, invalid channel name.", parv[1]);
	    return;
    }

    if (irc_join(parv[1], channelkey))
    {
        irc_notice(event->origin->nick, "Joined \2%s\2 successfully.", parv[1]);
        return;
    }

    return;
}
