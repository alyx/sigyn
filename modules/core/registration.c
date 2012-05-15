#include "sigyn.h"

DECLARE_MODULE("core/registration", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Matthew <mattwb65@gmail.com>");

static void handle_433(void *data, UNUSED void *udata);
static void handle_001(void *data, UNUSED void *udata);
static bool got_001 = false;

void _modinit(UNUSED module_t *m)
{
    mowgli_hook_associate("433", handle_433, NULL);
    mowgli_hook_associate("001", handle_001, NULL);
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    mowgli_hook_dissociate("433", handle_433);
    mowgli_hook_dissociate("001", handle_001);
}

static void handle_433(void *data, UNUSED void *udata)
{
    if (got_001)
        return;
    char *nickbuf;
    nickbuf = mowgli_alloc(BUFSIZE);
    strcpy(nickbuf, me.client->nick);
    strcat(nickbuf, "_");
    me.client->nick = mowgli_strdup(nickbuf);
    irc_nick(nickbuf);
    mowgli_free(nickbuf);
}

static void handle_001(void *data, UNUSED void *udata)
{
    got_001 = true;
}

