/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */
#ifndef __SIGYN_IRC_H
#define __SIGYN_IRC_H

#include <sys/types.h>

//typedef struct irc_channel_t irc_channel_t;
//typedef struct irc_user_t irc_user_t;

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

    size_t nchans;
    char **chans;
} irc_user_t;

typedef struct
{
    irc_user_t *origin;
    char *command;
    char *target;
    char *message;
} irc_event_t;

extern int raw(char *line, ...);
extern void irc_pass(char *password);
extern void irc_nick(char *nick);
extern void irc_user(char *user, char *host, char *server, char *real);
extern void irc_oper(char *user, char *password);
extern void irc_quit(char *message);
extern void irc_squit(char *server, char *message);
extern void irc_join(char *channel, char *key);
extern void irc_part(char *channel);
extern void irc_mode(char *target, char *modestring);
extern void irc_topic(char *channel, char *topic);
extern void irc_names(char *channel);
extern void irc_invite(char *user, char *channel);
extern void irc_kick(char *channel, char *user, char *comment);
extern void irc_version(char *server);
extern void irc_stats(char *query, char *server);
extern void irc_links(char *remote, char *mask);
extern void irc_time(char *server);
extern void irc_connect(char *target, int port, char *remote);
extern void irc_trace(char *server);
extern void irc_admin(char *server);
extern void irc_info(char *server);
extern void irc_privmsg(char *receiver, char *text);
extern void irc_notice(char *nickname, char *text);
extern void irc_who(char *name, char *o);
extern void irc_whois(char *server, char *nickmask);
extern void irc_whowas(char *nickname, int count, char *server);
extern void irc_kill(char *nickname, char *comment);
extern void irc_ping(char *server1, char *server2);
extern void irc_pong(char *daemon);
extern void irc_away(char *message);
extern void irc_users(char *server);
extern void irc_userhost(char *nickname);
#endif
