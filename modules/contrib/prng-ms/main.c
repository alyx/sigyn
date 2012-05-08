#include "sigyn.h"


DECLARE_MODULE("contrib/prng-ms", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Alyx <alyx@malkier.net>");

static void cmd_ms(const irc_event_t *event, int parc, char **parv);

static uint32_t square;

void _modinit(UNUSED module_t *m)
{
    square = ((int)time(NULL) * (int)getpid);
    command_add("ms", cmd_ms, 0, AC_NONE, "Returns a random number generated from a middle-square algorithm.", "[RELOAD]");
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("ms", cmd_ms);
}

static uint16_t ms_get(void)
{
    uint16_t i;

    i = (square >>8) & 0xffff;
    square = i*i;

    return i;
}

static void cmd_ms(const irc_event_t *event, int parc, char **parv)
{
    if (parc > 0)
    {
        if ((strcasecmp("RELOAD", parv[1])) == 0)
        {
            square = ((int)time(NULL) * (int)getpid);
            irc_notice(event->target, "\x02[Middle-Square]\x02 Seed reloaded.");
            return;
        }
        else
        {
            command_fail(CMD_BADPARAM, event->origin, "ms");
            return;
        }
    }
    
    irc_notice(event->origin->nick, "\x02[Middle-Square]\x02 Pseudo-random number: %d",
            ms_get());
}
