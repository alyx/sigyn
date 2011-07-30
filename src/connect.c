/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

void uplink_connect(char *uplink, int *port)
{
    struct addrinfo hints {
        int ai_family = AF_UNSPEC;
        int ai_socktype = SOCK_STREAM;
    };
    struct addrinfo *res;
    /*char *hostname = mowgli_alloc(256);*/
    char *hostname = malloc(256);
    gethostname(hostname, 255);

    switch( getaddrinfo( uplink, port, &hints, &res) )
    {
        case EAI_AGAIN:
            logger(LOG_WARNING, "Cannot resolve hostname: The name server returned a temporary failure indication. Try again later.");
            exit(0);
        case EAI_FAIL:
            logger(LOG_WARNING, "Cannot resolve hostname: The name server returned a permanent failure indication.");
            exit(0);
        case EAI_MEMORY:
            logger(LOG_WARNING, "Cannot resolve hostname: Out of memory.");
            exit(0);
        case EAI_SYSTEM:
            logger(LOG_WARNING, "Cannot resolve hostname: System returned error: %i", errno);
            exit(0);
    }

    logger(LOG_STATUS, "Attempting to connect to %s:%d", uplink, port);
    me.uplink.sock = socket(res.ai_family, res.ai_socktype, res.ai_protocol);

    if( connect(me.uplink.sock, res.ai_addr, res.ai_addrlen) != -1)
        logger(LOG_STATUS, "Connection to %s:%d successful", uplink, port);
    else
    {
        logger(LOG_STATUS, "Connection to %s:%d failed: %i", uplink, port, errno);
        exit(0);
    }
    if(UPLINK_PASS)
        irc_pass(UPLINK_PASS);
    irc_nick(me.nick);
    irc_user(me.client.nick, hostname, me.uplink.server, me.client.gecos);
}
