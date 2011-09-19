#include "sigyn.h"

DECLARE_MODULE("example/dns", 0, _modinit, _moddeinit,
        "1.0", "Alyx <alyx@malkier.net>");

static void cmd_dns(irc_event_t *event, int parc, char **parv);

void _modinit(module_t *m)
{
    command_add("dns", cmd_dns);
}

void _moddeinit(module_unload_intent_t intent)
{
    return;
}

static void cmd_dns(irc_event_t *event, int parc, char **parv)
{
    struct hostent *dns;

    printf("parc size: %d\n", parc);

    if (parc < 1)
    {
        irc_privmsg(event->origin->nick, "Incorrect usage. Try 'dns <hostname to resolve>'.");
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
