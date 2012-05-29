#include "sigyn.h"

DECLARE_MODULE("contrib/voicebot", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Alyx <alyx@malkier.net>");

static mowgli_list_t banlist;
static mowgli_list_t userlist;

static mowgli_heap_t * ban_heap;
static mowgli_heap_t * user_heap;

static void handle_join(void * data, UNUSED void * udata);
static void handle_part(void * data, UNUSED void * udata);
static void cmd_vb(const irc_event_t * event, int parc, char ** parv);

static char * voicechan;

struct no_voice
{
    char * hostmask;
    char * reason;
    time_t set;
    time_t length;
    mowgli_node_t * n;
};

struct new_user
{
    irc_user_t * user;
    time_t entered;
};

void _modinit(UNUSED module_t *m)
{
    mowgli_config_file_entry_t * channel;

    channel = config_find_entry(me.config->entries, "voicechan");
    if (channel != NULL && (channel->vardata != NULL))
    {
        voicechan = channel->vardata;
        mowgli_hook_associate("JOIN", handle_join, NULL);
        mowgli_hook_associate("PART", handle_part, NULL);
    }
    else
    {
        printf("Failures and stuff\n");
        return;
    }

    ban_heap  = mowgli_heap_create(sizeof(struct no_voice), 128, BH_NOW);
    user_heap = mowgli_heap_create(sizeof(struct new_user), 128, BH_NOW);

    if (ban_heap == NULL || user_heap == NULL)
        sigyn_fatal("voicebot: block allocator failed.");

    command_add("voicebot", cmd_vb, 1, AC_ADMIN, "Manages voicebot functions.",
            "<ban [!T <time>] <user> [reason]|voice [time]>");
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    mowgli_hook_dissociate("JOIN", handle_join);
    mowgli_hook_dissociate("PART", handle_part);
    command_del("voicebot", cmd_vb);
}

static void handle_join(void * data, UNUSED void * udata)
{
    irc_event_t * event;
    mowgli_node_t * n;
    struct no_voice * banned;
    struct new_user * new;

    event = (irc_event_t *)data;

    if (strcmp(event->target, voicechan) != 0)
        return;

    MOWGLI_ITER_FOREACH(n, banlist.head)
    {
        banned = (struct no_voice *)n->data;

        if (glob(banned->hostmask, event->origin->hostmask))
            return;
    }

    new = mowgli_heap_alloc(user_heap);

    new->user = mowgli_alloc(sizeof(irc_user_t));
    new->user->nick = mowgli_strdup(event->origin->nick);
    new->user->hostmask = mowgli_strdup(event->origin->hostmask);
    new->entered = time(NULL);


    mowgli_node_add(new, mowgli_node_create(), &userlist);
}

static void handle_part(void * data, UNUSED void * udata)
{
    irc_event_t * event;
    struct new_user * nu;
    mowgli_node_t * n;

    event = (irc_event_t *)data;

    if (strcmp(event->target, voicechan) != 0)
        return;

    MOWGLI_ITER_FOREACH(n, userlist.head)
    {
        nu = (struct new_user *)n->data;
        if (!strcmp(event->origin->nick, nu->user->nick))
            mowgli_node_delete(n, &userlist);
    }
}

void do_voice(void * userdata)
{
    char buf[256];
    struct new_user * u;
    mowgli_node_t * n;

    MOWGLI_ITER_FOREACH(n, userlist.head)
    {
        /*buf = "+v ";*/
        u = (struct new_user *)n->data;
        /*mowgli_strlcat(buf, u);*/
        snprintf(buf, 256, "+v %s", u->user->nick);
        irc_mode(voicechan, buf);
    }
}
    
void unban(void * userdata)
{
    char * buf;
    struct no_voice * banned;
    banned = (struct no_voice *)userdata;

    mowgli_node_delete(banned->n, &banlist);
    mowgli_free(banned->hostmask);
    mowgli_heap_free(ban_heap, banned);
}

static void cmd_vb_ban(const irc_event_t * event, int parc, char ** parv)
{
    struct no_voice * banned;
    char * reason;
    int i;
    char * buf;

    if (parv[2] == NULL)
    {
        // error here
    }

    banned = mowgli_heap_alloc(ban_heap);
    banned->hostmask = mowgli_strdup(parv[2]);
    banned->set = time(NULL);

    if (parv[3] != NULL && !strcmp(parv[3], "!T"))
    {
        if (parv[4] == NULL)
        {
            //error here
        }

        buf = mowgli_alloc(1024);
        snprintf(buf, 1024, "unban %s", banned->hostmask);
        banned->length = (time_t)strtol(parv[4], (char **)NULL, 10);
        timer_add(buf, unban, banned, banned->length, false);
        mowgli_node_add(banned, mowgli_node_create(), &banlist);
    }
}

static void cmd_vb_voice(const irc_event_t * event, int parc, char ** parv)
{
    time_t wait;

    if (parv[2] != NULL)
    {
        wait = (time_t)strtol(parv[2], (char **)NULL, 10);
        timer_add((char *)wait, do_voice, (void *)wait, wait, false); 
        return;
    }
}

static void cmd_vb(const irc_event_t * event, int parc, char ** parv)
{
    if (!strcmp(parv[1], "ban"))
    {
        printf("running ban\n");
        cmd_vb_ban(event, parc, parv);
        return;
    }

    else if (!strcmp(parv[1], "voice"))
    {
        printf("running voice\n");
        cmd_vb_voice(event, parc, parv);
        return;
    }

    return;
}
