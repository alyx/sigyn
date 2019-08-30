#include "sigyn.h"
#include <sys/resource.h>

DECLARE_MODULE("admin/memory", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Alyx Wolcott <alyx@malkier.net>");

static void cmd_memory(const irc_event_t *event, int parc, char **parv);

void _modinit(UNUSED module_t *m)
{
    command_add("memory", cmd_memory, 0, AC_ADMIN, "Returns Sigyn memory usage.", "<command>");
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("memory", cmd_memory);
}

static void cmd_memory(const irc_event_t *event, int parc, char **parv)
{
    struct rusage* mem = mowgli_alloc(sizeof(struct rusage));
    getrusage(RUSAGE_SELF, mem);

    command_reply(event->target, "Memory Usage (Max RSS): %.2fMB", (float)(mem->ru_maxrss/1024.0));
}

