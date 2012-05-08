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

static void cmd_raw(const irc_event_t *event, int parc, char **parv)
{
    int i;
    char *buffer;

    if ((strcmp(config_find_entry(me.config->entries, "admin")->vardata, event->origin->nick)) != 0)
        return;

    buffer = mowgli_alloc(BUFSIZE);

    for (i = 1; i <= parc; i++)
    {
        printf("Adding %s to buffer.\n", parv[i]);
        /*snprintf(buffer, BUFSIZE, "%s %s", buffer, parv[i]);*/
        strlcat(buffer, parv[i], BUFSIZE);
        if (i != parc)
            strlcat(buffer, " ", BUFSIZE);
        printf("New buffer: %s\n", buffer);
    }

    printf("Buffer: %s\n", buffer);
    raw(buffer);
}
