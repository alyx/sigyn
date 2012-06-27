#include "sigyn.h"

DECLARE_MODULE("admin/raw", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Alyx <alyx@malkier.net>");

static void cmd_raw(const irc_event_t *event, int parc, char **parv);

void _modinit(UNUSED module_t *m)
{
    command_add("raw", cmd_raw, 1, AC_ADMIN, "Sends a raw command to the IRC server.", "<command>");
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("raw", cmd_raw);
}

static void cmd_raw(UNUSED const irc_event_t *event, int parc, char **parv)
{
    int i;
    char *buffer;

    buffer = mowgli_alloc(BUFSIZE);

    for (i = 1; i <= parc; i++)
    {
        logger(LOG_DEBUG, "[admin/raw]: Adding %s to buffer.", parv[i]);
        mowgli_strlcat(buffer, parv[i], BUFSIZE);
        if (i != parc)
            mowgli_strlcat(buffer, " ", BUFSIZE);
        logger(LOG_DEBUG, "[admin/raw]: New buffer: %s", buffer);
    }

    logger(LOG_DEBUG, "[admin/raw]: Final buffer: %s", buffer);
    raw(buffer);
    mowgli_free(buffer);
}
