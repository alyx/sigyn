/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

#include <signal.h>

    /* XXX: I would almost think make to make SIGHUP this rehash the config,
     * but we really don't have a reason to rehash things, I think.
     * Thought needed here before development.
     */

static void signal_hup_handler(int signum)
{
  logger(LOG_GENERAL, "Received HUP, rehashing.");
  
  char config[BUFSIZE];
  snprintf(config, BUFSIZE, "%s/%s", SYSCONFDIR, "sigyn.conf");
  
  mowgli_config_file_free(me.config);
  me.config = mowgli_config_file_load(config);
}

static void signal_int_handler(int signum)
{
    sigyn_fatal("caught interrupt");
}

static void signal_segv_handler(int signum)
{
#ifdef ENABLE_STACK
    sigyn_log_backtrace();
#endif
    sigyn_fatal("Segmentation fault.");
}

static void signal_usr1_handler(int signum)
{
    sigyn_fatal("Out of memory!");
}

void signals_init(void)
{
    mowgli_signal_install_handler(SIGHUP, signal_hup_handler);
    mowgli_signal_install_handler(SIGINT, signal_int_handler);
    mowgli_signal_install_handler(SIGTERM, SIG_IGN);

    mowgli_signal_install_handler(SIGUSR1, signal_usr1_handler);
    mowgli_signal_install_handler(SIGSEGV, signal_segv_handler);
}

