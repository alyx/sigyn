#include "sigyn.h"

DECLARE_MODULE("admin/part", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Justin Crawford <Justasic@gmail.com>");

static void cmd_join(const irc_event_t *event, int parc, char **parv);

void _modinit(UNUSED module_t *m)
{
    command_add("part", cmd_join, 1, AC_ADMIN, "Parts an IRC channel", "<channel> [<reason>]");
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("part", cmd_join);
}


static void cmd_join(const irc_event_t *event, int parc, char **parv)
{
    char *message = mowgli_alloc(BUFSIZE);
    int i;
    // Started at two so we don't get the command garbage, we only want the message!
    for(i = 2; parv[i] != NULL; ++i)
    {
      strlcat(message, parv[i], BUFSIZE);
      strlcat(message, " ", BUFSIZE);
    }
    // There's always a space at the end of the string, we don't want it!
    size_t len = strlen(message);
    if(isspace(message[len-1]))
      message[len-1] = 0x00;
    
    irc_part(parv[1], message);
    irc_notice(event->origin->nick, "Parted \2%s\2 successfully", parv[1]);
    mowgli_free(message);
}
