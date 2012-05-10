#include "sigyn.h"

DECLARE_MODULE("core/ping", MODULE_UNLOAD_CAPABILITY_NEVER, _modinit, _moddeinit,
        "1.0", "Alyx <alyx@malkier.net>");

static void handle_ping(void *data, UNUSED void *udata);

void _modinit(UNUSED module_t *m)
{
    mowgli_hook_associate("PING", handle_ping, NULL);
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    mowgli_hook_dissociate("PING", handle_ping);
}

static void handle_ping(void *data, UNUSED void *udata)
{
    irc_event_t *event;
    event = (irc_event_t *)data;

    irc_pong(event->data + 1);
}
