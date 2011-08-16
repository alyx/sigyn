/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

void initialise_sigyn(char *nick, char *ident, char *gecos, char *uplink, int port)
{
    me.stats.start = time(NULL);
    me.stats.inB = 0;
    me.stats.outB = 0;
    me.client->nick = nick;
    me.client->user = ident;
    me.client->gecos = gecos;
    me.uplink.port = port;
    me.uplink.hostname = uplink;
#ifdef _WIN32
    me.uplink.winsock = false;
#endif
}

int main(int argc, char *argv[])
{
    initialise_sigyn(SIGYN_NICK, SIGYN_NICK, SIGYN_REALNAME, UPLINK_SERVER, UPLINK_PORT);
    uplink_connect(me.uplink.hostname, me.uplink.port);

    char text[513];
    irc_event_t *event = mowgli_alloc(sizeof(irc_event_t));
    while (1)
    {
        /*fread(text, 1, 512, me.uplink.sock);*/
        //XXX: Redo how we get the contents, probably do some shiny select() wrapper.
        event = parse(text);
    }
    close_socket(me.uplink.sock);
    return 0;
}
