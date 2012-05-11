#include "sigyn.h"

DECLARE_MODULE("admin/exec", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Matthew <mattwb65@gmail.com>");

static void cmd_exec(const irc_event_t *event, int parc, char **parv);

void _modinit(UNUSED module_t *m)
{
    command_add("exec", cmd_exec, 1, AC_ADMIN, "Executes a command on Sigyn's system.", "<command>");
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("exec", cmd_exec);
}

static void cmd_exec(const irc_event_t *event, int parc, char **parv)
{
    int i;
    char *buffer;
    FILE *fp;
    int status;
    char data[512];

    buffer = mowgli_alloc(BUFSIZE);

    for (i = 1; i <= parc; i++)
    {
        mowgli_strlcat(buffer, parv[i], BUFSIZE);
        if (i != parc)
            mowgli_strlcat(buffer, " ", BUFSIZE);
    }
    fp = popen(buffer, "r");
    if (fp == NULL)
    {
        irc_privmsg(event->target, "Couldn't open pipe for output");
        return;
    }
    while (fgets(data, sizeof(data), fp) != NULL)
    {
        irc_privmsg(event->target, "%s", data);
    }
    pclose(fp);
    mowgli_free(buffer);
}
