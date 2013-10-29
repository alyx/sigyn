/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

mowgli_heap_t *channel_heap;
mowgli_heap_t *chanuser_heap;

static void handle_join(void *data, UNUSED void *udata);
static void handle_part(void *data, UNUSED void *udata);
static void handle_kick(void *data, UNUSED void *udata);
static void handle_quit(void *data, UNUSED void *udata);
static void handle_topic(void *data, UNUSED void *udata);
static void handle_332(void *data, UNUSED void *udata);
static void handle_352(void *data, UNUSED void *udata);
static void handle_329(void *data, UNUSED void *udata);
static void handle_nick(void *data, UNUSED void *udata);
static void handle_mode(void *data, UNUSED void *udata);

/*
 * Routine Description:
 * This routine searches through the channel list to locate
 * a specified channel.
 *
 * Arguments:
 *     name - A string containing the name of the channel to search for.
 *
 * Return value:
 *     c - Returns an irc_channel_t object if the channel is located,
 *         and returns NULL if it is not.
 *
 */

irc_channel_t * channel_find(const char * name)
{
    irc_channel_t * c;
    mowgli_node_t * n;

    MOWGLI_ITER_FOREACH(n, channels.head)
    {
        c = (irc_channel_t *)n->data;

        if (!strcasecmp(c->name, name))
            return c;
    }

    return NULL;
}

/*
 * Routine Description:
 * This routine searches through the channel user list to locate
 * a specified user.
 *
 * Arguments:
 *     channel  - The irc_channel_t structure of the channel to search in.
 *     nickname - A string containing the name of the user to search for.
 *
 * Return value:
 *     u - Returns a chanuser_t object if the user is located,
 *         and returns NULL if it is not.
 *
 */

chanuser_t * chanuser_find(irc_channel_t * c, const char * nickname)
{
    chanuser_t * cu;
    mowgli_node_t * n;

    MOWGLI_ITER_FOREACH(n, c->users.head)
    {
        cu = (chanuser_t *)n->data;

        if (!strcasecmp(cu->name, nickname))
            return cu;
    }

    return NULL;
}

/*
 * Routine Description:
 * This routine initialises the channel and chnauser memory heaps.
 *
 * Arguments:
 *     None
 *
 * Return value:
 *      None
 *
 */

void channel_init(void)
{
    channel_heap = mowgli_heap_create(sizeof(irc_channel_t), 1024, BH_NOW);
    chanuser_heap = mowgli_heap_create(sizeof(chanuser_t), 1024, BH_NOW);
    
    if (channel_heap == NULL || chanuser_heap == NULL)
        sigyn_fatal("channel_init(): block allocator failed.");
        
    mowgli_hook_associate("JOIN", handle_join, NULL);
    mowgli_hook_associate("PART", handle_part, NULL);
    mowgli_hook_associate("KICK", handle_kick, NULL);
    mowgli_hook_associate("QUIT", handle_quit, NULL);
    mowgli_hook_associate("TOPIC", handle_topic, NULL);
    mowgli_hook_associate("NICK", handle_nick, NULL);
    mowgli_hook_associate("332", handle_332, NULL);
    mowgli_hook_associate("352", handle_352, NULL);
    mowgli_hook_associate("329", handle_329, NULL);
    mowgli_hook_associate("MODE", handle_mode, NULL);
}

/*
 * Routine Description:
 * This routine provides a function to create a channel and append it to
 * the channel list.
 *
 * Arguments:
 *     name     - A string containing the name of the channel.
 * 
 * Return value:
 *     None
 *
 */

void channel_add(const char *name)
{
    irc_channel_t *c;

    if ((channel_find(name)) != NULL)
    {
        logger(LOG_DEBUG, "Channel %s already exists. Aborting channel_add().", name);
        return;
    }

    c = mowgli_heap_alloc(channel_heap);

    c->name = mowgli_strdup(name);

    mowgli_node_add(c, &c->node, &channels);
    logger(LOG_DEBUG, "channel_add(): Added channel %s", c->name);
}

/*
 * Routine Description:
 * This routine provides a function to create a channel user and append it to
 * the channel user list.
 *
 * Arguments:
 *     c           - The irc_channel_t structure of the channel.
 *     nickname    - A string containing the name of the user.
 * 
 * Return value:
 *     None
 *
 */

void chanuser_add(irc_channel_t *c, const char *nickname)
{
    chanuser_t *cu;

    if ((chanuser_find(c, nickname)) != NULL)
    {
        logger(LOG_DEBUG, "User %s exists in %s. Aborting chanuser_add().", nickname, c->name);
        return;
    }

    cu = mowgli_heap_alloc(chanuser_heap);

    cu->name = mowgli_strdup(nickname);
    cu->channel = c;
    c->nusers++;

    mowgli_node_add(cu, &cu->node, &c->users);

    logger(LOG_DEBUG, "chanuser_add(): Added user %s to %s", cu->name, c->name);
}

/*
 * Routine Description:
 * This routine provides a function to remove a channel from the channel list.
 *
 * Arguments:
 *     c     - The irc_user_t structure of the channel.
 *
 * Return value:
 *     None
 *
 */

void channel_del(irc_channel_t *c)
{
    mowgli_node_t *n, *tn;

    MOWGLI_ITER_FOREACH_SAFE(n, tn, c->users.head)
    {
        chanuser_del(c, ((chanuser_t *)n->data)->name);
    }
    mowgli_free(c->name);
    mowgli_free(c->topic);
    mowgli_node_delete(&c->node, &channels);
    mowgli_heap_free(channel_heap, c);
}

/*
 * Routine Description:
 * This routine provides a function to remove a channel user from the channel user list.
 *
 * Arguments:
 *     c           - The irc_user_t structure of the channel.
 *     nickname    - A string containing the name of the user.
 *
 * Return value:
 *     None
 *
 */

void chanuser_del(irc_channel_t *c, const char *nickname)
{
    chanuser_t *cu;

    cu = chanuser_find(c, nickname);
    if (cu == NULL)
    {
        logger(LOG_DEBUG, "User %s doesn't exist in %s. Aborting chanuser_del().", nickname, c->name);
        return;
    }
    logger(LOG_DEBUG, "chanuser_del(): Deleting user %s from %s", nickname, c->name);
    c->nusers--;
    mowgli_free(cu->name);
    mowgli_node_delete(&cu->node, &c->users);
    mowgli_heap_free(chanuser_heap, cu);
}

/*
 * Routine Description:
 * This routine provides a function to remove a channel user from all channels they're in.
 *
 * Arguments:
 *     nickname    - A string containing the name of the user.
 *
 * Return value:
 *     None
 *
 */

void chanuser_del_from_all(const char *nickname)
{
    irc_channel_t *c;
    chanuser_t *cu;
    mowgli_node_t *n, *tn;
    
    MOWGLI_ITER_FOREACH_SAFE(n, tn, channels.head)
    {
        c = (irc_channel_t *)n->data;
        mowgli_node_t *n2, *tn2;
        MOWGLI_ITER_FOREACH_SAFE(n2, tn2, c->users.head)
        {
            cu = (chanuser_t *)n2->data;
            if (strcmp(cu->name, nickname) == 0)
            {
                chanuser_del(c, cu->name);
            }
        }
    }
}

static void handle_join(void *data, UNUSED void *udata)
{
    irc_event_t *event;
    event = (irc_event_t *)data;
    /* Check if the user is us. */
    if (strcmp(me.client->nick, event->origin->nick) == 0)
    {
        channel_add(event->target);
        me.stats.channels++;
        irc_who(event->target, NULL);
        irc_mode(event->target, NULL);
        return;
    }
    irc_channel_t *channel;
    channel = channel_find(event->target);
    chanuser_add(channel, event->origin->nick);
}

static void handle_part(void *data, UNUSED void *udata)
{
    irc_event_t *event;
    event = (irc_event_t *)data;
    irc_channel_t *c;
    c = channel_find(event->target);
    if (c == NULL)
        return;
    // Check if the user is us.
    if (strcmp(me.client->nick, event->origin->nick) == 0)
    {
        channel_del(c);
        me.stats.channels--;
        return;
    }
    // channel_del will delete all users for us.
    chanuser_del(c, event->origin->nick);
}

static void handle_quit(void *data, UNUSED void *udata)
{
    irc_event_t *event;
    event = (irc_event_t *)data;
    chanuser_del_from_all(event->origin->nick);
}

static void handle_topic(void *data, UNUSED void *udata)
{
    irc_event_t *event;
    irc_channel_t *channel;
    event = (irc_event_t *)data;
    channel = channel_find(event->target);
    if (channel == NULL)
        return;
    channel->topic = mowgli_strdup(event->data);
}

static void handle_kick(void *data, UNUSED void *udata)
{
    int parc;
    char *parv[MAXPARC + 1], *tmp;
    INIT_PARV(parv, MAXPARC + 1);
    irc_event_t *event;
    irc_channel_t *c;
    event = (irc_event_t *)data;
    tmp = mowgli_strdup(event->data);
    parc = tokenize(tmp, parv);
    c = channel_find(event->target);
    if (c == NULL)
        return;
    // Check if the user is us.
    if (strcmp(me.client->nick, parv[0]) == 0)
    {
        channel_del(c);
        me.stats.channels--;
        return;
    }
    // channel_del will delete all users for us.
    chanuser_del(c, parv[0]);
    mowgli_free(tmp);
}

static void handle_332(void *data, UNUSED void *udata)
{
    int parc;
    irc_event_t *event;
    irc_channel_t *channel;
    char *parv[MAXPARC + 1], *tmp;
    INIT_PARV(parv, MAXPARC + 1);
    event = (irc_event_t *)data;
    tmp = mowgli_strdup(event->data);
    parc = tokenize(event->data, parv);
    channel = channel_find(parv[0]);
    if (channel == NULL)
        return;
    char topicbuf[BUFSIZE];
    int i;
    topicbuf[0] = '\0';
    parv[1] = parv[1]+1;
    for(i = 1; i <= parc; i++)
    {
        if (i != 1)
            mowgli_strlcat(topicbuf, " ", sizeof(topicbuf));
        mowgli_strlcat(topicbuf, parv[i], sizeof(topicbuf));
    }
    channel->topic = mowgli_strdup(topicbuf);
    mowgli_free(tmp);
}

static void handle_352(void *data, UNUSED void *udata)
{
    int parc;
    irc_event_t *event;
    irc_channel_t *channel;
    char *parv[MAXPARC + 1], *tmp;
    INIT_PARV(parv, MAXPARC + 1);
    event = (irc_event_t *)data;
    tmp = mowgli_strdup(event->data);
    parc = tokenize(tmp, parv);
    channel = channel_find(parv[0]);
    if (channel == NULL)
        return;
    chanuser_add(channel, parv[4]);
    mowgli_free(tmp);
}

static void handle_nick(void *data, UNUSED void *udata)
{
    irc_channel_t *c;
    irc_event_t *event;
    chanuser_t *cu;
    event = (irc_event_t *)data;
    mowgli_node_t *n, *tn;
    if (strcmp(me.client->nick, event->origin->nick) == 0)
        me.client->nick = mowgli_strdup(event->target);
    MOWGLI_ITER_FOREACH_SAFE(n, tn, channels.head)
    {
        c = (irc_channel_t *)n->data;
        mowgli_node_t *n2, *tn2;
        MOWGLI_ITER_FOREACH_SAFE(n2, tn2, c->users.head)
        {
            cu = (chanuser_t *)n2->data;
            if (strcmp(cu->name, event->origin->nick) == 0)
                cu->name = event->target;
        }
    }
}

/* :blah!blah@blah MODE #blah :+o blah */ 
static void handle_mode(void *data, UNUSED void *udata)
{
    irc_channel_t *c;
    irc_event_t *event;
    event = (irc_event_t *)data;
    // It's not even worth our time if the target isn't a channel
    if (!ischannel(event->target))
        return;
    // Do we know about this channel?
    if ((c = channel_find(event->target)) == NULL)
    {
        logger(LOG_DEBUG, "Mode set on a channel I don't know about");
        return;
    }
    else
    {
        // XXX: We need mode handling
        logger(LOG_DEBUG, "Modes set on %s: %s\n", c->name, event->data);
    }
}

static void handle_329(void *data, UNUSED void *udata)
{
    irc_event_t *event;
    irc_channel_t *c;
    int parc;
    char *parv[MAXPARC + 1], *tmp;
    INIT_PARV(parv, MAXPARC + 1);
    event = (irc_event_t *)data;
    tmp = mowgli_strdup(event->data);
    parc = tokenize(tmp, parv);
    if ((c = channel_find(parv[0])) == NULL)
    {
        logger(LOG_DEBUG, "Couldn't find channel %s\n", parv[0]);
        return;
    }
    else
    {
        int time = atoi(parv[1]);
        logger(LOG_DEBUG, "Channel %s was created at %i\n", c->name, time);
        c->created = (time_t)time;
    }
}
