#include "sigyn.h"

DECLARE_MODULE("core/ctcpbasics", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Justin Crawford <Justasic@gmail.com>");

static void handle_ctcp_version(void *data, UNUSED void *udata);
static void handle_ctcp_time(void *data, UNUSED void *udata);
static void handle_ctcp_ping(void *data, UNUSED void *udata);

void _modinit(module_t *m)
{
    MODULE_TRY_REQUEST_DEPENDENCY(m, "core/ctcp");
    mowgli_hook_associate("CTCP VERSION", handle_ctcp_version, NULL);
    mowgli_hook_associate("CTCP TIME", handle_ctcp_time, NULL);
    mowgli_hook_associate("CTCP PING", handle_ctcp_ping, NULL);
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    mowgli_hook_dissociate("CTCP VERSION", handle_ctcp_version);
    mowgli_hook_dissociate("CTCP TIME", handle_ctcp_time);
    mowgli_hook_dissociate("CTCP PING", handle_ctcp_ping);
}

static void handle_ctcp_version(void *data, UNUSED void *udata)
{
    irc_event_t *event;
    event = (irc_event_t *)data;
    ctcp_reply(event->origin->nick, "VERSION Sigyn %s", SIGYN_VERSION);
}

static void handle_ctcp_time(void *data, UNUSED void *udata)
{
    irc_event_t *event;
    event = (irc_event_t *)data;
    struct tm *ptm;
    time_t rawtime;
    time(&rawtime);
    char buf[100];
    ptm = localtime(&rawtime);
    strftime(buf, 100, "TIME %Z %c", ptm);
    ctcp_reply(event->origin->nick, buf);
}

static void handle_ctcp_ping(void *data, UNUSED void *udata)
{
    irc_event_t *event;
    event = (irc_event_t *)data;
    ctcp_reply(event->origin->nick, "PING %d", time(NULL));
}
