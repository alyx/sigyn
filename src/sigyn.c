/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"
#include "config.h"
#include "protocol/general.h"

int main(int argc, char *argv[])
{
    switch( getaddrinfo( UPLINK_SERVER, UPLINK_PORT, &hints, &res) )
    switch( getaddrinfo( me.uplink.hostname, me.uplink.port, &me.uplink.hints, &me.uplink.res) )
    {
        case EAI_AGAIN:
            log(LOG_WARNING, "getaddrinfo error: The name server returned a temporary failure indication. Try again later.");
            exit(0);
        case EAI_FAIL:
            log(LOG_WARNING, "getaddrinfo error: The name server returned a permanent failure indication.");
            exit(0);
        case EAI_MEMORY:
            log(LOG_WARNING, "getaddrinfo error: Out of memory.");
            exit(0);
        case EAI_SYSTEM:
            log(LOG_WARNING, "getaddrinfo error: System returned error: %i", errno);
            exit(0);
    }
    log(LOG_STATUS, "Attempting to connect to %s:%d.", me.uplink.hostname, me.uplink.port);
    me.uplink.sock = socket(me.uplink.res.ai_family, me.uplink.res.ai_socktype, me.uplink.res.ai_protocol);
    if( connect(me.uplink.sock, me.uplink.res.ai_addr, me.uplink.res.ai_addrlen) != -1)
    {
        log(LOG_STATUS, "Connection to %s:%d successful.", me.uplink.hostname, me.uplink.port);
    }
    else
    {
        log(LOG_STATUS, "Connection to %s:%d failed: %i", me.uplink.hostname, me.uplink.port, errno);
        exit(0);
    }
    char *hostname[1024];
    hostname[1023] = "\0";
    gethostname(hostname, 1023);
    user(me.nick, hostname, me.uplink.server, SIGYN_REALNAME);
    nick(me.nick);
    
    char *text = mowgli_alloc(513);
    while (1)
    {
         recv_data(text);
         parse(text);
    }
    
    return 0;
}
