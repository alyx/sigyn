/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 *
 * Abstract: This module handles connecting to the IRC server.
 */

#include "sigyn.h"

static mowgli_list_t connection_list;

#ifdef _WIN32
    WSADATA wsaData;
#endif

static inline int set_nb(socket_t fd)
{
#ifdef _WIN32
    unsigned long flags = 1;
    if(ioctlsocket(fd, FIONBIO, &flags) != 0)
        return -1;
#else
    int flags;

    flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;

    if (fcntl(fd, F_SETFL, flags))
        return -1;
#endif

    return 1;
}

/*
 * Routine Description:
 * This routine provides a simple function for initialising Winsock.
 *
 * Arguments:
 *     None
 *
 * Return value:
 *     wsres - 0 for "Everything worked", otherwise whatever value is
 *     returned by WSAStartup().
 *
 */

int StartWSA(void)
{
#ifdef _WIN32
    if(me.uplink.winsock == true)
        return 0;
    int wsres = WSAStartup(MAKEWORD(2,2), &wsaData);
    switch(wsres)
    {
        case 0:
            logger(LOG_DEBUG, "Successfully started Winsock.");
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
    return wsres;
#else
    return 0;
#endif
}

/*
 * Routine Description:
 * This routine provides an operating-system agnostic method of discovering
 * the hostname of the system.
 *
 * Arguments:
 *     host - A pointer to a string filled with the hostname by gethostname().
 *     len  - The maximum length we allow the hostname to be.
 *
 * Return value:
 *     res - sigyn_hostname() forwards the return value of gethostname().
 *           This is 0 for success, and -1 for failure.
 *
 */

int sigyn_hostname(char *host, int len)
{
    int res;
    StartWSA();
    res = gethostname(host, len);
    return res;
}

/*
 * Routine Description:
 * This routine connect to the IRC server and initialises the IRC registration phase.
 *
 * Arguments:
 *     uplink - A string containing the hostname of the server to connect to.
 *     port   - An integer stating the port to connect to.
 *     vhost  - A string containing the hostname for Sigyn to bind to (Optional).
 *
 * Return value:
 *     sock - Returns the socket_t object (a SOCKET on Windows, a file descriptor (int) everywhere else).
 *
 */

socket_t uplink_connect(char *uplink, uint16_t port, char *vhost)
{
	socket_t sock;
	int error, flags;
    unsigned int optval;
    struct addrinfo *res = NULL,
                    hints;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    StartWSA();

    if ((error = getaddrinfo(uplink, NULL, &hints, &res)))
        sigyn_fatal("Cannot resolve hostname (%s): %s", uplink, gai_strerror(error));
    if (res->ai_addr == NULL)
    {
        freeaddrinfo(res);
        sigyn_fatal("Cannot resolve hostname (%s).");
    }

    logger(LOG_DEBUG, "Attempting to connect to %s:%d", uplink, port);
    if (!(sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)))
    {
        freeaddrinfo(res);
        sigyn_fatal("Unable to create socket.");
    }

    if (sock > me.maxfd)
        me.maxfd = sock;

    if (vhost != NULL)
    {
        struct addrinfo *bindres = NULL;
        
        if ((error = getaddrinfo(vhost, NULL, &hints, &bindres)))
        {
            freeaddrinfo(res);
            close_portable(sock);
            sigyn_fatal("Cannot resolve vhost (%s): %s", vhost, gai_strerror(error));
        }
        if (bindres->ai_addr == NULL)
        {
            freeaddrinfo(res);
            freeaddrinfo(bindres);
            sigyn_fatal("Cannot resolve vhost (%s).");
        }

        optval = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));

        if (bind(sock, bindres->ai_addr, bindres->ai_addrlen) < 0)
        {
            freeaddrinfo(res);
            freeaddrinfo(bindres);
            close_portable(sock);
            sigyn_fatal("Unable to bind to vhost (%s): %s", vhost, strerror(errno));
        }

        freeaddrinfo(bindres);
    }
    else
        logger(LOG_DEBUG, "No vhost found. Not trying to set one.");

    set_nb(sock);

    switch(res->ai_family)
    {
        case AF_INET:
            ((struct sockaddr_in *) res->ai_addr)->sin_port = htons(port);
            logger(LOG_DEBUG, "Socket is IPv4.");
            break;
        case AF_INET6:
            ((struct sockaddr_in6 *) res->ai_addr)->sin6_port = htons(port);
            logger(LOG_DEBUG, "Socket is IPv6.");
            break;
    }

    if ((connect(sock, res->ai_addr, res->ai_addrlen) == -1)
            && errno != EINPROGRESS && errno != EINTR)
    {
        close_portable(sock);
        freeaddrinfo(res);
        if (vhost)
            sigyn_fatal("Failed to connect to %s (Using vhost %s): %s",
                    uplink, vhost, strerror(errno));
        else
            sigyn_fatal("Failed to connect to %s: %s", uplink, strerror(errno));
    }

    freeaddrinfo(res);

    return sock;
}

/*
 * Routine Description:
 * This routine disconnects from the IRC server.
 *
 * Arguments:
 *     None.
 *
 * Return value:
 *     None.
 */

void uplink_disconnect(void)
{
    if(me.uplink.connected == true)
        close_portable(me.uplink.sock);
}
