/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#ifndef SIGYN_CHANNEL_H
#define SIGYN_CHANNEL_H

#include "mowgli.h"
#include "irc.h"

mowgli_list_t channels;
mowgli_list_t chanusers;

typedef struct chanuser_ chanuser_t;

struct chanuser_
{
    irc_channel_t *channel;
    char *name;
    unsigned int modes;
    mowgli_node_t node;

};

/* channel status modes */
#define CSTATUS_NONE   0x00 /* obvious */
#define CSTATUS_VOICE  0x01 /* v */
#define CSTATUS_HALFOP 0x02 /* h */
#define CSTATUS_OP     0x04 /* o */
#define CSTATUS_ADMIN  0x08 /* a */
#define CSTATUS_OWNER  0x10 /* q on unreal/inspircd u on sporksircd */

/* channel list modes */
#define LMODE_BAN       0x00000 /* b */
#define LMODE_EXCEPTION 0x00001 /* e */
#define LMODE_INVITE    0x00002 /* I */
#define LMODE_QUIET     0x00004 /* q */

/* channel modes */
#define CMODE_INVITE    0x00000 /* i */
#define CMODE_TOPIC     0x00001 /* t */
#define CMODE_EXTMSG    0x00002 /* n */
#define CMODE_KEY       0x00004 /* k */
#define CMODE_FLOOD     0x00008 /* f on unreal/inspircd/etc */
#define CMODE_FORWARD   0x00010 /* f on charybdis and friends */
#define CMODE_MODERATED 0x00012 /* m */
#define CMODE_SECRET    0x00014 /* s */
#define CMODE_PRIVATE   0x00016 /* p */


/* macros to check status */
#define is_voice(x) ((x) && (x)->modes & CSTATUS_VOICE)
#define is_halfop(x) ((x) && (x)->modes & CSTATUS_HALFOP)
#define is_op(x) ((x) && (x)->modes & CSTATUS_OP)
#define is_admin(x) ((x) && (x)->modes & CSTATUS_ADMIN)
#define is_owner(x) ((x) && (x)->modes & CSTATUS_OWNER)

extern void channel_init(void);
extern irc_channel_t *channel_find(const char *name);
extern chanuser_t *chanuser_find(irc_channel_t *channel, const char *nickname);
extern void channel_add(const char *name);
extern void chanuser_add(irc_channel_t *channel, const char *nickname);
extern void channel_del(irc_channel_t *name);
extern void chanuser_del(irc_channel_t *channel, const char *nickname);

#endif /* SIGYN_CHANNEL_H */
