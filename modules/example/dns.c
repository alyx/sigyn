#include "sigyn.h"

DECLARE_MODULE("example/dns", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Matthew <mattwb65@gmail.com>");

static void cmd_dns(const irc_event_t *event, int parc, char **parv);
static void resolve_cb(mowgli_dns_reply_t *reply, int reason, void *vptr);
static mowgli_dns_t *resolver;

typedef struct
{
    char *domain;
    const irc_event_t *event;
    mowgli_dns_query_t query;
} query_data;

void _modinit(UNUSED module_t *m)
{
    command_add("dns", cmd_dns, 1, AC_NONE, "Resolves the IP address(es) of a host.", "<host>");
    resolver = mowgli_dns_create(me.ev, MOWGLI_DNS_TYPE_ASYNC);
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("dns", cmd_dns);
    mowgli_dns_destroy(resolver);
}

static void resolve_cb(mowgli_dns_reply_t *reply, int reason, void *vptr)
{
    char buf[2048];
    query_data *data = vptr;
    const void *sockptr;

    if (reply == NULL)
    {
        switch (reason)
        {
            case MOWGLI_DNS_RES_NXDOMAIN:
                command_reply(data->event->target, "DNS Resolution Failed: NXDOMAIN");
                break;
            case MOWGLI_DNS_RES_INVALID:
                command_reply(data->event->target, "DNS Resolution Failed: Invalid domain.");
                break;
            case MOWGLI_DNS_RES_TIMEOUT:
                command_reply(data->event->target, "DNS Resolution Failed: Timed out.");
                break;
        }
        goto end;
    }

    if (reply->addr.addr.ss_family == AF_INET)
    {
        const struct sockaddr_in *saddr = (const struct sockaddr_in *)&reply->addr.addr;
        sockptr = &saddr->sin_addr;
    }
    else
    {
        command_reply(data->event->target, "DNS Resolution Failed: Invalid Address family %d", reply->addr.addr.ss_family);
        return;
    }
    inet_ntop(reply->addr.addr.ss_family, sockptr, buf, sizeof(buf));
    command_reply(data->event->target, "%s resolved to %s", data->domain, buf);

end:
    mowgli_free(data->domain);
    mowgli_free(vptr);
}

static void cmd_dns(const irc_event_t *event, int parc, char **parv)
{
    query_data *data = mowgli_alloc(sizeof(query_data));
    mowgli_dns_query_t *query = &data->query;
    query->callback = resolve_cb;
    query->ptr = data;
    data->domain = mowgli_strdup(parv[1]);
    data->event = event;

    mowgli_dns_gethost_byname(resolver, data->domain, query, MOWGLI_DNS_T_A);
}
