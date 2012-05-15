#include "sigyn.h"

DECLARE_MODULE("example/dns", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Alyx <alyx@malkier.net>");

static void cmd_dns(const irc_event_t *event, int parc, char **parv);

void _modinit(UNUSED module_t *m)
{
    command_add("dns", cmd_dns, 1, AC_NONE, "Resolves the IP address(es) of a host.", "<host>");
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("dns", cmd_dns);
}

static void cmd_dns(const irc_event_t *event, int parc, char **parv)
{
    int i;
    char buf[BUFSIZE];
    struct hostent *dns;

    dns = gethostbyname(parv[1]);

    if (dns == NULL)
    {
        command_reply(event->target, "DNS resolution failed.");
        return;
    }

    buf[0] = '\0';
    for (i = 0; dns->h_addr_list[i] != NULL; i++)
    {
        mowgli_strlcat(buf, " ", sizeof(buf));
        mowgli_strlcat(buf, inet_ntoa(*(struct in_addr *)(dns->h_addr_list[i])), sizeof(buf));
    }
    command_reply(event->target, "%s (%i):%s", dns->h_name, i, buf);


}
