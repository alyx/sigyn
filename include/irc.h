/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#pragma once

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

int raw(char *line, ...);
void irc_pass(char *password);
void irc_nick(char *nick);
void irc_user(char *user, char *host, char *server, char *real);
void irc_oper(char *user, char *password);
void irc_quit(char *message);
void irc_squit(char *server, char *message);
void irc_join(char *channel, char *key);
void irc_part(char *channel);
void irc_mode(char *target, char *modestring);
void irc_topic(char *channel, char *topic);
void irc_names(char *channel);
void irc_invite(char *user, char *channel);
void irc_kick(char *channel, char *user, char *comment);
void irc_version(char *server);
void irc_stats(char *query, char *server);
void irc_links(char *remote, char *mask);
void irc_time(char *server);
void irc_connect(char *target, int port, char *remote);
void irc_trace(char *server);
void irc_admin(char *server);
void irc_info(char *server);
void irc_privmsg(char *receiver, char *text);
void irc_notice(char *nickname, char *text);
void irc_who(char *name, char *o);
void irc_whois(char *server, char *nickmask);
void irc_whowas(char *nickname, int count, char *server);
void irc_kill(char *nickname, char *comment);
void irc_ping(char *server1, char *server2);
void irc_pong(char *daemon);
void irc_away(char *message);
void irc_users(char *server);
void irc_userhost(char *nickname);
