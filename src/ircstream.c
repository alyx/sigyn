/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 *
 * Abstract: This module handles communications with the IRCd.
 */

#include "sigyn.h"

int irc_read(socket_t sock, char *buffer)
{
    int n;

    n = read(sock, buffer, BUFSIZE);
    buffer[n] = '\0';

    me.stats.inB += n;
    
    return n;
}

void sigyn_introduce_client(char *nick)
{
	/* Generate the system hostname; RFC1459 wants us to send this. */
	char hostname[256];
    sigyn_hostname(hostname, 255);
    
    /* Introduce ourselves to the server. */
    irc_nick(nick);
    irc_user(nick, hostname, me.uplink.hostname, me.client->gecos);
}
