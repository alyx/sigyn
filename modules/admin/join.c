#include "sigyn.h"

static void cmd_join(const irc_event_t *event, int parc, char **parv);
static void cmd_part(const irc_event_t *event, int parc, char **parv);

DECLARE_MODULE("admin/join",
               MODULE_UNLOAD_CAPABILITY_OK,
               join_init,
               join_fini,
               "1.0",
               "Justin Crawford <Justasic@gmail.com>");

void
join_init(UNUSED module_t *m)
{
    command_add("join",
                cmd_join,
                1,
                AC_ADMIN,
                "Joins an IRC channel",
                "<channel> [<key>]");

    command_add("part",
                cmd_part,
                1,
                AC_ADMIN,
                "Parts an IRC channel",
                "<channel> [<reason>]");
}

void
join_fini(UNUSED module_unload_intent_t intent)
{
    command_del("join", cmd_join);
    command_del("part", cmd_part);
}

static void
cmd_part(const irc_event_t *event, int parc, char **parv)
{
    if (irc_part(parv[1], message))
    {
        irc_notice(event->origin->nick, "Parted \2%s\2 successfully", parv[1]);
        return;
    }

    return;
}

static void
cmd_join(const irc_event_t *event, int parc, char **parv)
{
    if (irc_join(parv[1], (parc > 2) ? parv[2] : NULL))
    {
        irc_notice(event->origin->nick, "Joined \2%s\2 successfully", parv[1]);
        return;
    }

    return;
}
