/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

int raw(char *line, ...) {
    char *sendbuf[510];
    va_list args;
    
    va_start(args, line);
    vsnprintf(sendbuf, 509, line, args);
    va_end(args);

    strlcpy(sendbuf, "\r\n");

    int sent = send(socket, sendbuf, strlen(sendbuf), 0);
    me.stats.outB += sent;
    char *logout = snprintf("<< %s", sendbuf);
    log(LOG_RAW, logout);
    return sent;
}

static void irc_nick(char *newnick)
{
    raw("NICK %s", newnick);
}

static void irc_user(char *user, char *host, char *server, char *real)
{
    raw("USER %s %s %s :%s", user, host, server, real);
}

static void irc_quit(char *message)
{
    raw("QUIT :%s", message);
}

static void irc_pong(char *message)
{
    raw("PONG :%s", message);
}

static void irc_privmsg(char *target, char *message)
{
    raw("PRIVMSG %s :%s", target, message);
}

static void irc_notice(char *target, char *message)
{
    raw("NOTICE %s: %s", target, message);
}


