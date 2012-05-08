#include "sigyn.h"

DECLARE_MODULE("admin/modload", MODULE_UNLOAD_CAPABILITY_OK, _modinit, _moddeinit,
        "1.0", "Matthew <mattwb65@gmail.com>");

static void cmd_modload(const irc_event_t *event, int parc, char **parv);
static void cmd_modunload(const irc_event_t *event, int parc, char **parv);

void _modinit(UNUSED module_t *m)
{
    command_add("modload", cmd_modload, 1, AC_ADMIN, "Loads a module.", "<name>");
    command_add("modunload", cmd_modunload, 1, AC_ADMIN, "Unloads a module.", "<name>");
}

void _moddeinit(UNUSED module_unload_intent_t intent)
{
    command_del("modload", cmd_modload);
    command_del("modunload", cmd_modunload);
}


static void cmd_modload(const irc_event_t *event, int parc, char **parv)
{
    module_t *mod;

    if ((strcmp(config_find_entry(me.config->entries, "admin")->vardata, event->origin->nick)) != 0)
      return;

    if (module_find_published(parv[1]))
    {
        irc_notice(event->origin->nick, "Module \2%s\2 is already loaded.", parv[1]);
        return;
    }

    mod = module_load(parv[1]);

    if (mod == NULL)
        irc_notice(event->origin->nick, "Module \2%s\2 failed to load.", parv[1]);
    else
        irc_notice(event->origin->nick, "Module \2%s\2 successfully loaded.", parv[1]);
}

static void cmd_modunload(const irc_event_t *event, int parc, char **parv)
{
    module_t *mod;

    if ((strcmp(config_find_entry(me.config->entries, "admin")->vardata, event->origin->nick)) != 0)
      return;

    mod = module_find_published(parv[1]);
    if (!mod)
    {
        irc_notice(event->origin->nick, "Module \2%s\2 is not loaded.", parv[1]);
        return;
    }
    if (mod->can_unload != MODULE_UNLOAD_CAPABILITY_OK)
    {
        irc_notice(event->origin->nick, "Module \2%s\2 is a permanent module and cannot be unloaded.");
        return;
    }

    module_unload(mod, MODULE_UNLOAD_INTENT_PERM);

    irc_notice(event->origin->nick, "Module \2%s\2 successfully unloaded.", parv[1]);
}

