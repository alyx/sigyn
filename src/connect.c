/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 *
 * Abstract: This module handles connecting to the IRC server.
 */

#include "sigyn.h"

#ifdef _WIN32
    WSADATA wsaData;
#endif

void uplink_connect(char *uplink, int port)
{
    char hostname[256];
    sigyn_hostname(hostname, 255);
    struct addrinfo *res, hints;
//    hints = { 0; };
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

#ifdef _WIN32
    int wsres = WSAStartup(MAKEWORD(2,2), &wsaData);
    switch(wsres)
    {
        case 0:
            logger(LOG_STATUS, "Successfully started Winsock.");
            me.uplink.winsock = true;
        case WSASYSNOTREADY:
            sigyn_fatal("Cannot start winsock: Underlying network is not ready for communication.");
        case WSAVERNOTSUPPORTED:
            /* Note: This should never happen. */
            sigyn_fatal("Cannot start winsock: This version of winsock is not supported by this winsock implementation.");
        case WSAEPROCLIM:
            sigyn_fatal("Cannot start winsock: Winsock task limit reached.")
        default:
            sigyn_fatal("Cannot start winsock: Unknown errno %d", wsres);
    }
#endif

    int result;
    result = getaddrinfo( uplink, (char*)port, &hints, &res);
    switch(result)
    {
        case 0:
            logger(LOG_STATUS, "Hostname resolution successful");
        case EAI_AGAIN:
            sigyn_fatal("Cannot resolve hostname: The name server returned a temporary failure indication. Try again later.");
        case EAI_FAIL:
            sigyn_fatal("Cannot resolve hostname: The name server returned a permanent failure indication.");
        case EAI_MEMORY:
            sigyn_fatal("Cannot resolve hostname: Out of memory.");
        case EAI_SYSTEM:
            sigyn_fatal("Cannot resolve hostname: System returned error: %i", ERRNO);
        default:
            sigyn_fatal("Cannot resolve hostname: Unknown error %d", result);
    }

    logger(LOG_STATUS, "Attempting to connect to %s:%d", uplink, port);
    me.uplink.sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if( connect(me.uplink.sock, res->ai_addr, res->ai_addrlen) != -1)
        logger(LOG_STATUS, "Connection to %s:%d successful", uplink, port);
    else
    {
        logger(LOG_STATUS, "Connection to %s:%d failed: %i", uplink, port, ERRNO);
        sigyn_fatal("Connection failed");
    }

#ifdef UPLINK_PASS
        irc_pass(UPLINK_PASS);
#endif
    irc_nick(me.client->nick);
    irc_user(me.client->nick, hostname, me.uplink.hostname, me.client->gecos);
}
