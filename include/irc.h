/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#ifndef __SIGYN_IRC_H
#define __SIGYN_IRC_H

#include <sys/types.h>

typedef struct {
    char *name;
    char *topic;
    size_t nusers;
    char **users;

    size_t nmodes;
    struct {
        char c;
        size_t nparam;
        char **users;
    } *modes;
} irc_channel_t;

typedef struct
{
    char *nick;
    char *user;
    char *host;
    char *gecos;

    bool server;

    size_t nchans;
    char **chans;
} irc_user_t;

typedef struct
{
    irc_user_t *origin;
    char *command;
    char *target;
    char *data;
} irc_event_t;

extern int raw(char *line, ...);
extern int read_irc(socket_t sock, char *buffer);
extern void sigyn_introduce_client(char *nick);

extern void irc_pass(const char *password);
extern void irc_nick(const char *nick);
extern void irc_user(const char *user, const char *host, const char *server, const char *real);
extern void irc_oper(const char *user, const char *password);
extern void irc_quit(const char *message);
extern void irc_squit(const char *server, const char *message);
extern void irc_join(const char *channel, const char *key);
extern void irc_part(const char *channel, const char *message);
extern void irc_mode(const char *target, const char *modestring);
extern void irc_topic(const char *channel, const char *topic);
extern void irc_names(const char *channel);
extern void irc_invite(const char *user, const char *channel);
extern void irc_kick(const char *channel, const char *user, const char *comment);
extern void irc_version(const char *server);
extern void irc_stats(const char *query, const char *server);
extern void irc_links(const char *remote, const char *mask);
extern void irc_time(const char *server);
extern void irc_connect(const char *target, int port, const char *remote);
extern void irc_trace(const char *server);
extern void irc_admin(const char *server);
extern void irc_info(const char *server);
extern void irc_privmsg(const char *receiver, const char *format, ...);
extern void irc_notice(const char *nickname, const char *format, ...);
extern void irc_who(const char *name, const char *o);
extern void irc_whois(const char *server, const char *nickmask);
extern void irc_whowas(const char *nickname, int count, const char *server);
extern void irc_kill(const char *nickname, const char *comment);
extern void irc_ping(const char *server1, const char *server2);
extern void irc_pong(const char *daemon);
extern void irc_away(const char *message);
extern void irc_users(const char *server);
extern void irc_userhost(const char *nickname);

#endif /* __SIGYN_IRC_H */
