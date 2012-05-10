/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#ifndef SIGYN_CHANNEL_H
#define SIGYN_CHANNEL_H

#include "mowgli.h"

mowgli_list_t channels;
mowgli_list_t chanusers;

typedef struct channel_ channel_t;
typedef struct chanuser_ chanuser_t;

struct channel_
{
    char *name;
    char *topic;
    unsigned int modes;
    unsigned int usercount;
    mowgli_list_t users;
    mowgli_node_t node;
};

struct chanuser_
{
    channel_t *channel;
    char *name;
    unsigned int modes;
    mowgli_node_t node;

};

/* channel status modes */
#define CSTATUS_NONE   0x00
#define CSTATUS_VOICE  0x01
#define CSTATUS_HALFOP 0x02
#define CSTATUS_OP     0x04
#define CSTATUS_ADMIN  0x08
#define CSTATUS_OWNER  0x10

/* macros to check status */
#define is_voice(x) ((x) && (x)->modes & CSTATUS_VOICE)
#define is_halfop(x) ((x) && (x)->modes & CSTATUS_HALFOP)
#define is_op(x) ((x) && (x)->modes & CSTATUS_OP)
#define is_admin(x) ((x) && (x)->modes & CSTATUS_ADMIN)
#define is_owner(x) ((x) && (x)->modes & CSTATUS_OWNER)

extern void channel_init(void);
extern channel_t *channel_find(const char *name);
extern chanuser_t *chanuser_find(const char *channel, const char *nickname);
extern void channel_add(const char *name);
extern void chanuser_add(const char *channel, const char *nickname);
extern void channel_del(const char *name);
extern void chanuser_del(const char *channel, const char *nickname);

#endif /* SIGYN_CHANNEL_H */
