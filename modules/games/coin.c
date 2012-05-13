#include "sigyn.h"

DECLARE_MODULE("games/coin", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Matthew <mattwb65@gmail.c>");

static void cmd_coin(const irc_event_t * event, UNUSED int parc, UNUSED char **parv);

void _modinit(UNUSED module_t *m)
{
    command_add("coin", cmd_coin, 0, AC_NONE, "Flips a coin.", NULL);
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("coin", cmd_coin);
}

static void cmd_coin(const irc_event_t * event, UNUSED int parc, UNUSED char **parv)
{
    int result = rand() % 2;
    irc_privmsg(event->target, "\001ACTION flips a coin.\001");
    if (result == 0)
        irc_privmsg(event->target, "The coin lands, and it's... heads.");
    else
        irc_privmsg(event->target, "The coin lands, and it's... tails.");
}
