#include "sigyn.h"

DECLARE_MODULE("games/eightball", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Alyx <alyx@malkier.net>");

static void cmd_eightball(const irc_event_t * event, UNUSED int parc, 
        UNUSED char **parv);

static const char *responses[20] = {
    "It is certain.",
    "It is decidedly so.",
    "Without a doubt.",
    "Yes -- definitely.",
    "You may rely on it.",
    "As I see it, yes.",
    "Most likely.",
    "Outlook good.",
    "Yes.",
    "Signs point to yes.",
    "Reply hazy, try again.",
    "Ask again later.",
    "Better not tell you now.",
    "Cannot predict now.",
    "Concentrate and ask again.",
    "Don't count on it.",
    "My reply is no.",
    "My sources say no.",
    "Outlook not so good.",
    "Very doubtful."
};

void _modinit(UNUSED module_t *m)
{
    command_add("eightball", cmd_eightball, 0, AC_NONE,
            "Ask the 8-ball a question.", "[question]");
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("eightball", cmd_eightball);
}

static void cmd_eightball(const irc_event_t * event, UNUSED int parc,
        UNUSED char **parv)
{
    command_reply(event->target, "%s", responses[(rand() % 20)]);
}
