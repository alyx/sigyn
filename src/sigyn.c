/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"
#include "protocol/general.h"

void initialise_sigyn(char *nick, char *ident, char *gecos, char *uplink, int port)
{
    me.client.nick = nick;
    me.client.user = ident;
    me.client.gecos = gecos;
    me.uplink.port = port;
    me.uplink.hostname = uplink;
}

int main(int argc, char *argv[])
{
    initialise_sigyn(SIGYN_NICK, SIGYN_NICK, SIGYN_REALNAME, UPLINK_SERVER, UPLINK_PORT);
    irc_connect(me.uplink.hostname, me.uplink.port);

    if(UPLINK_PASS)
        irc_pass(UPLINK_PASS);
    irc_nick(me.nick);
    irc_user(me.client.nick, hostname, me.uplink.server, me.client.gecos);
    
    char *text = mowgli_alloc(513);
    irc_event_t *event = mowgli_alloc(sizeof(irc_event_t));
    while (1)
    {
        fread(text, 1, 512, sock);
        event = parse(text);
    }
    
    return 0;
}
