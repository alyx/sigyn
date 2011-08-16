/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 *
 * Abstract: This module handles connecting to the IRC server.
 */

#include "sigyn.h"
#include "platform.h"

#ifdef _WIN32
#   include <winsock2.h>
#   include <ws2tcpip.h>
#endif

#include <string.h>
#include <stdio.h>
#include <errno.h>

#ifndef _WIN32
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <netdb.h>
#   include <arpa/inet.h>
#   include <unistd.h>
#   define close_portable close
#else
#   define snprintf sprintf_s
#   define close_portable closesocket
#endif


#ifdef _WIN32
    WSADATA wsaData;
#endif

/*
 * Routine Description: 
 * This routine connect to the IRC server and initialises the IRC registration phase.
 *
 * Arguments:
 *     uplink - A string containing the hostname of the server to connect to.
 *     port   - The port to connect to, this defaults to 6667.
 *
 *  Return value:
 *      None.
 *
 */

void uplink_connect(char *uplink, int port)
{
    char hostname[256];
    sigyn_hostname(hostname, 255);
    struct addrinfo *res = NULL,
                    *ptr = NULL,
                    hints;
    
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (port == 0)
        port = 6667;

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
    result = getaddrinfo(uplink, (char*)port, &hints, &res);
    if (result != 0)
        sigyn_fatal("Cannot resolve hostname (%s): %s", uplink, gai_strerror(result));

    logger(LOG_STATUS, "Attempting to connect to %s:%d", uplink, port);
    ptr = res;
    me.uplink.sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    if( connect(me.uplink.sock, res->ai_addr, res->ai_addrlen) != -1)
        logger(LOG_STATUS, "Connection to %s:%d successful", uplink, port);
    else
    {
        logger(LOG_STATUS, "Connection to %s:%d failed: %i", uplink, port, ERRNO);
        sigyn_fatal("Connection failed");
    }

    freeaddrinfo(res);

#ifdef UPLINK_PASS
        irc_pass(UPLINK_PASS);
#endif
    irc_nick(me.client->nick);
    irc_user(me.client->nick, hostname, me.uplink.hostname, me.client->gecos);
}
