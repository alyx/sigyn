#include "sigyn.h"

DECLARE_MODULE("examples/hello", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Alyx <alyx@malkier.net>");

static void handle_privmsg(void *data, UNUSED void *udata);
static void cmd_hello(const irc_event_t *event, UNUSED int parc, UNUSED char **parv);
static void timed_cmd_hello(UNUSED void *argument);
static mowgli_eventloop_timer_t *hello_timer = NULL;

void _modinit(UNUSED module_t *m)
{
    mowgli_hook_associate("PRIVMSG", handle_privmsg, NULL);
    command_add("hello", cmd_hello, 0, AC_NONE, "says hello to you!", "<user>");
    hello_timer = mowgli_timer_add(me.ev, "hello", timed_cmd_hello, NULL, 10);
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    mowgli_hook_dissociate("PRIVMSG", handle_privmsg);
    command_del("hello", cmd_hello);
    mowgli_timer_destroy(me.ev, hello_timer);
}

static void handle_privmsg(void *data, UNUSED void *udata)
{
    irc_event_t *event;
    event = (irc_event_t *)data;

    if ((strcmp(event->target, event->origin->nick)) == 0)
        irc_privmsg(event->target, "Hello %s!", event->target);
}

static void cmd_hello(const irc_event_t *event, UNUSED int parc, UNUSED char **parv)
{
    command_reply(event->target, "Hello %s!", event->origin->nick);
}

static void timed_cmd_hello(UNUSED void *argument)
{
    logger(LOG_DEBUG, "Oh look, this timer says hai thar!");
}
