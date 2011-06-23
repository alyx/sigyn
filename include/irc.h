/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#pragma once

typedef struct irc_channel_t irc_channel_t;
typedef struct irc_user_t irc_user_t;

struct irc_channel_t {
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
};

struct irc_user_t
{
    char *nick;
    char *user;
    char *host;
    char *gecos;

    size_t nchans;
    char **chans;
};

struct irc_event_t
{
    irc_user_t *origin;
    char *command;
    char *target;
    char *message;
};

int raw(char *line, ...);
static void irc_nick(char *nick);
static void irc_user(char *user, char *host, char *server, char *gecos);
static void irc_quit(char *message);
static void irc_pong(char *message);
static void irc_privmsg(char *target, char *message);
static void irc_notice(char *target, char *message);
