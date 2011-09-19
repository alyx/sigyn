#include "sigyn.h"

DECLARE_MODULE("ping", 0, _modinit, _moddeinit,
        "1.0", "Alyx <alyx@malkier.net>");

static void handle_ping(void *data, void *unused);

void _modinit(module_t *m)
{
    mowgli_hook_associate("PING", handle_ping, NULL);
}

void _moddeinit(module_unload_intent_t intent)
{
    mowgli_hook_dissociate("PING", handle_ping);
}

static void handle_ping(void *data, void *unused)
{
    irc_event_t *event;
    event = (irc_event_t *)data;

   irc_pong(event->data + 1);
}
