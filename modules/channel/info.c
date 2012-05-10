#include "sigyn.h"

DECLARE_MODULE("channel/info", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Matthew <mattwb65@gmail.com>");

static void cmd_info(const irc_event_t *event, UNUSED int parc, UNUSED char **parv);

void _modinit(UNUSED module_t *m)
{
    command_add("info", cmd_info, 0, AC_NONE, "Returns information on the channel the command is executed in.", NULL);
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("info", cmd_info);
}

static void cmd_info(const irc_event_t *event, UNUSED int parc, UNUSED char **parv)
{
    irc_channel_t * c;

    if (!ischannel(event->target))
        return;
    c = channel_find(event->target);
    if (c == NULL)
        irc_privmsg(event->target, "Unable to find channel.");
    else
        irc_privmsg(event->target, "Name: %s Users: %i Topic: %s", c->name, c->nusers, c->topic);
}

