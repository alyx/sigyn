#include "sigyn.h"

DECLARE_MODULE("example/dns", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Alyx <alyx@malkier.net>");

static void cmd_dns(const irc_event_t *event, int parc, char **parv);

void _modinit(UNUSED module_t *m)
{
    command_add("dns", cmd_dns, 1, "Resolves the IP address(es) of a host.", "<host>");
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("dns", cmd_dns);
}

static void cmd_dns(const irc_event_t *event, int parc, char **parv)
{
    struct hostent *dns;

    if (parc < 1)
    {
        command_fail(CMD_NEEDSPARAM, event->origin, "dns");
        return;
    }

    dns = gethostbyname(parv[1]);

    if (dns == NULL)
    {
        irc_privmsg(event->target, "DNS resolution failed.");
        return;
    }

    char buf[BUFSIZE];
    int i;
    buf[0] = '\0';
    while (dns->h_addr_list[i] != NULL) {
        mowgli_strlcat(buf, " ", sizeof(buf));
        mowgli_strlcat(buf, inet_ntoa(*(struct in_addr *)(dns->h_addr_list[i])), sizeof(buf));
        i++;
    }
    irc_privmsg(event->target, "%s (%i):%s", dns->h_name, i, buf);


}
