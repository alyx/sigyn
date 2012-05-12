/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include <sigyn.h>

/*
 * Routine Description:
 * This routine sends a raw line of IRC data to the socket specified in me.uplink.sock.
 *
 * Arguments:
 *     line - A string containing the (printf-formatted) string to send
 *            to the IRCd.
 *     ...  - An undefined amount of additional arguments inserted into 
 *            the formatted string (line).
 *
 * Return value:
 *     length - An integer stating the length of the send string.
 *
 */

int raw(char *line, ...)
{
    char sendbuf[510];
    int length;
    va_list args;

    va_start(args, line);
    vsnprintf(sendbuf, 509, line, args);
    va_end(args);

    mowgli_strlcat(sendbuf, "\r\n", 510);
    length = strlen(sendbuf);

    sendq_add(me.uplink.sock, sendbuf, length); 
    strip(sendbuf, "\r\n");
    logger(LOG_RAW, "<< %s", sendbuf);
    return length;
}

/*
 * Routine Description:
 * This routine reads from the specified socket into a buffer.
 *
 * Arguments:
 *     sock   - A socket_t object stating the socket to read from.
 *     buffer - A chunk of allocated memory stating where to insert the string
 *              read from the socket.
 *
 * Return value:
 *     n - An integer stating the amount of data read from the socket.
 *
 */

int read_irc(socket_t sock, char *buffer)
{
    int n;

    n = read(sock, buffer, BUFSIZE);
    buffer[n] = '\0';

    me.stats.inB += n;
    
    return n;
}

/*
 * Routine Description:
 * This routine introduces the IRC client to the server.
 *
 * Arguments:
 *     nick - A string containing the nickname to use.
 *
 * Return value:
 *     None
 *
 */

void sigyn_introduce_client(char *nick)
{
	/* Generate the system hostname; RFC1459 wants us to send this. */
	char hostname[256];
    sigyn_hostname(hostname, 255);
    
    /* Introduce ourselves to the server. */
    irc_nick(nick);
    irc_user(nick, hostname, me.uplink.hostname, me.client->gecos);
}

static void serv_optional(const char *server, const char *command)
{
    /* Many commands can take a sole
     * optional servername as a param.
     * This function simplifies the process.
     */
    if (server != NULL)
        raw("%s %s", command, server);
    else
        raw("%s", command);
}

/* RFC 1459 commands follow.
 * RFC 1459 is the original
 * IRC RFC by J. Oikarinen.
 * http://www.ietf.org/rfc/rfc1459.txt
 */

/* 4.1: Connection registration. */

void irc_pass(const char *password)
{
    /* 4.1.1: PASS
     * Parameters: <password>
     * Example: PASS secretpasswordhere
     */
    raw("PASS %s", password);
}

void irc_nick(const char *nick)
{
    /* 4.1.2: NICK
     * Parameters: <nickname> [ <hopcount> ]
     * Example: NICK Wiz
     */
    raw("NICK %s", nick);
}

void irc_user(const char *user, const char *host, const char *server, const char *real)
{
    /* 4.1.3: USER
     * Parameters: <username> <hostname> <servername> <realname>
     * Example: USER guest tolmoon tolsun :Ronnie Reagan
     */
    raw("USER %s %s %s :%s", user, host, server, real);
}

void irc_oper(const char *user, const char *password)
{
    /* 4.1.5: OPER
     * Parameters: <user> <password>
     * Example: OPER foo bar
     */
    raw("OPER %s %s", user, password);
}

void irc_quit(const char *message)
{
    /* 4.1.6: QUIT
     * Parameters: [<Quit message>]
     * Example: QUIT :Gone to have lunch
     */
    if (message == NULL)
        raw("QUIT");
    else
        raw("QUIT :%s", message);
}

void irc_squit(const char *server, const char *message)
{
    /* 4.1.7: SQUIT
     * Parameters: <server> <comment>
     * Example: SQUIT tolsun.oulu.fi :Bad Link
     */
    raw("SQUIT %s :%s", server, message);
}

/* 4.2: Channel operations */

void irc_join(const char *channel, const char *key)
{
    /* 4.2.1: JOIN
     * Parameters: <channel>{,<channel>} [<key>{,<key>}]
     * Example: JOIN #foo,#bar fubar,foobar 
     */
    if (key != NULL)
        raw("JOIN %s %s", channel, key);
    else
        raw("JOIN %s", channel);
}

void irc_part(const char *channel, const char *message)
{
    /* 4.2.2: PART
     * Parameters: <channel>{,<channel>}
     * Example: PART #twilight_zone
     */
    if (message != NULL)
        raw("PART %s :%s", channel, message);
    else
        raw("PART %s", channel);
}

void irc_mode(const char *target, const char *modestring)
{
    /* 4.2.3: MODE
     *  4.2.3.1: Channel modes
     *    Parameters: <channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>] [<ban mask>]
     *    Example: MODE #Finnish +im
     *  4.2.3.2: User modes
     *    Parameters: <nickname> {[+|-]|i|w|s|o}
     *    Example: MODE WiZ -o
     */
    /* XXX: Stop being lazy and code a real mode command. */
    raw("MODE %s %s", target, modestring);
}

void irc_topic(const char *channel, const char *topic)
{
    /* 4.2.4: TOPIC
     * Parameters: <channel> [<topic>]
     * TOPIC #test :another topic
     */
    if (topic != NULL)
        raw("TOPIC %s :%s", channel, topic);
    else
        raw("TOPIC %s", channel);
}

void irc_names(const char *channel)
{
    /* 4.2.5: NAMES
     * Parameters: [<channel>{,<channel>}]
     * Example: NAMES #twilight_zone,#42
     */
    raw("NAMES %s", channel);
}

void irc_invite(const char *user, const char *channel)
{
    /* 4.2.7: INVITE
     * Parameters: <nickname> <channel>
     * Example: INVITE WiZ #Twilight_Zone
     */
    raw("INVITE %s %s", user, channel);
}

void irc_kick(const char *channel, const char *user, const char *comment)
{
    /* 4.2.8: KICK
     * Parameters: <channel> <user> [<comment>]
     * Example: KICK #Finnish John :Speaking English
     */
    if (comment != NULL)
        raw("KICK %s %s :%s", channel, user, comment);
    else
        raw("KICK %s %s", channel, user);
}

/* 4.3: Server queries and commands */

void irc_version(const char *server)
{
    /* 4.3.1: VERSION
     * Parameters: [<server>]
     * Example: VERSION tolsun.oulu.fi 
     */
    if (server != NULL)
        raw("VERSION %s", server);
    else
        raw("VERSION");
}

void irc_stats(const char *query, const char *server)
{
    /* 4.3.2: STATS
     * Parameters: [<query> [<server>]]
     * Example: STATS c eff.org
     */
    if (server != NULL)
        raw("STATS %s %s", query, server);
    else
        raw("STATS %s", query);
}

void irc_links(const char *remote, const char *mask)
{
    /* 4.3.3: LINKS
     * Parameters: [[<remote server>] <server mask>]
     * LINKS *.bu.edu *.edu
     */
    if (remote != NULL && mask != NULL)
        raw("LINKS %s %s", remote, mask);
    else if (remote != NULL && mask == NULL)
        raw("LINKS %s", remote);
    else if (remote == NULL)
        raw("LINKS");
}

void irc_time(const char *server)
{
    /* 4.3.4: TIME
     * Parameters: [<server>]
     * Example: TIME tolsun.oulu.fi  
     */
    serv_optional(server, "TIME");
}

void irc_connect(const char *target, int port, const char *remote)
{
    /* 4.3.5: CONNECT
     * Parameters: <target server> [<port> [<remote server>]]
     * Example: CONNECT tolsun.oulu.fi
     */
    if (remote != NULL && port != 0)
        raw("CONNECT %s %d %s", target, port, remote);
    else if (remote != NULL && port == 0)
        raw("CONNECT %s %s", target, remote);
    else
        raw("CONNECT %s", target);
}

void irc_trace(const char *server)
{
    /* 4.3.6: TRACE
     * Parameters: [<server>]
     * Example: TRACE *.oulu.fi
     */
    serv_optional(server, "TRACE");
}

void irc_admin(const char *server)
{
    /* 4.3.7: ADMIN
     * Parameters: [<server>]
     * Example: ADMIN tolsun.oulu.fi
     */
    serv_optional(server, "ADMIN");
}

void irc_info(const char *server)
{
    /* 4.3.8: INFO
     * Parameters: [<server>]
     * Example: INFO csd.bu.edu
     */
    serv_optional(server, "INFO");
}

/* 4.4: Sending messages */

void irc_privmsg(const char *receiver, const char *format, ...)
{
    /* 4.4.1: PRIVMSG
     * Parameters: <receiver>{,<receiver>} <text to be sent>
     * Example: PRIVMSG Angel :yes I'm receiving it!
     */

    char buf[BUFSIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, BUFSIZE, format, args);
    va_end(args);

    raw("PRIVMSG %s :%s", receiver, buf);
}

void irc_notice(const char *nickname, const char *format, ...)
{
    /* 4.4.2: NOTICE
     * Parameters: <nickname> <text>
     * Example: NOTICE Angel :Foobar
     */

    char buf[BUFSIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, BUFSIZE, format, args);
    va_end(args);

    raw("NOTICE %s :%s", nickname, buf);
}

/* 4.5: User based queries */

void irc_who(const char *name, const char *o)
{
    /* 4.5.1: WHO
     * Parameters: [<name> [<o>]]
     * Example: WHO *.fi
     */
    if (o != NULL)
        raw("WHO %s %s", name, o);
    else
        raw("WHO %s", name);
}

void irc_whois(const char *server, const char *nickmask)
{
    /* 4.5.2: WHOIS
     * Parameters: [<server>] <nickmask>[,<nickmask>[,...]]
     * Example: WHOIS eff.org trillian
     */
    if (server != NULL)
        raw("WHOIS %s %s", server, nickmask);
    else
        raw("WHOIS %s", nickmask);
}

void irc_whowas(const char *nickname, int count, const char *server)
{
    /* 4.5.3: WHOWAS
     * Parameters: <nickname> [<count> [<server>]]
     * WHOWAS Trillian 1 *.edu
     */
    if (count != 0 && server != NULL)
        raw("WHOWAS %s %d %s", nickname, count, server);
    else if (count != 0 && server == NULL)
        raw("WHOWAS %s %d", nickname, count);
    else if (count == 0 && server != NULL)
        raw("WHOWAS %s %s", nickname, server);
    else
        raw("WHOWAS %s", nickname);
}

/* 4.6: Miscellaneous messages */

void irc_kill(const char *nickname, const char *comment)
{
    /* 4.6.1: KILL
     * Parameters: <nickname> <comment>
     * KILL Foo :Random reason
     */
    raw("KILL %s :%s", nickname, comment);
}

void irc_ping(const char *server1, const char *server2)
{
    /* 4.6.2: PING
     * Parameters: <server1> [<server2>]
     * PING tolsun.oulu.fi 
     */
    if (server2 != NULL)
        raw("PING %s %s", server1, server2);
    else
        raw("PING %s", server1);
}

void irc_pong(const char *daemon)
{
    /* 4.6.3: PONG
     * Parameters: <daemon> [<daemon2>]
     * PONG tolsun.oulu.fi
     */
    raw("PONG %s", daemon);
}

/* 5: Optionals */

void irc_away(const char *message)
{
    /* 5.1: AWAY
     * Parameters: [message]
     * Example: AWAY :Gone to lunch.  Back in 5
     */
    if (message != NULL)
        raw("AWAY :%s", message);
    else
        raw("AWAY");
}

void irc_users(const char *server)
{
    /* 5.5: USERS
     * Parameters: [<server>]
     * Example: USERS eff.org
     */
    serv_optional(server, "USERS");
}

void irc_userhost(const char *nickname)
{
    /* 5.7: USERHOST
     * Parameters: <nickname>{<space><nickname>}
     * USERHOST Wiz Michael Marty p
     */
    raw("USERHOST %s", nickname);
}

