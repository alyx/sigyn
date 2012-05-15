#include "sigyn.h"

DECLARE_MODULE("channel/info", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Matthew <mattwb65@gmail.com>");

static void cmd_info(const irc_event_t *event, UNUSED int parc, UNUSED char **parv);

void _modinit(UNUSED module_t *m)
{
    command_add("info", cmd_info, 0, AC_NONE, "Returns information on the channel the command is executed in.", "[channel]");
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("info", cmd_info);
}

static void cmd_info(const irc_event_t *event, UNUSED int parc, UNUSED char **parv)
{
    irc_channel_t * c;

    if (parc == 0)
    {
        if (ischannel(event->target))
            c = channel_find(event->target);
        else
        {
            command_reply(event->target, "Without a parameter, this command can only be executed in a channel.");
            return;
        }
    }
    else
        c = channel_find(parv[1]);

    if (c == NULL)
        command_reply(event->target, "Unable to find channel.");
    else
        command_reply(event->target, "Name: %s Users: %i Topic: %s", c->name, c->nusers, c->topic);
}

