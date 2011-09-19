#include "sigyn.h"

DECLARE_MODULE("admin/raw", 0, _modinit, _moddeinit,
        "1.0", "Alyx <alyx@malkier.net>");

static void cmd_raw(irc_event_t *event, int parc, char **parv);

void _modinit(module_t *m)
{
    command_add("raw", cmd_raw);
}

void _moddeinit(module_unload_intent_t intent)
{
    command_del("raw", cmd_raw);
}

static void cmd_raw(irc_event_t *event, int parc, char **parv)
{
    int i;
    char *buffer;

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
