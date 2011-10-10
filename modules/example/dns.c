#include "sigyn.h"

DECLARE_MODULE("example/dns", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Alyx <alyx@malkier.net>");

static void cmd_dns(irc_event_t *event, int parc, char **parv);

void _modinit(UNUSED module_t *m)
{
    command_add("dns", cmd_dns, "Resolves the IP address of a domain.", "<domain name>");
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("dns", cmd_dns);
}

static void cmd_dns(irc_event_t *event, int parc, char **parv)
{
    struct hostent *dns;

    printf("parc size: %d\n", parc);

    if (parc < 1)
    {
        command_fail(CMD_NEEDSPARAM, event->origin, "dns");
        return;
    }

    dns = gethostbyname(parv[1]);

    if (dns == NULL)
    {
        irc_privmsg(event->origin->nick, "DNS resolution failed.");
        return;
    }
    else
        irc_privmsg(event->target, "%s = %s", dns->h_name, inet_ntoa(*(struct in_addr *)dns->h_addr_list[0]));
}
