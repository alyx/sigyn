#include "sigyn.h"

#include <signal.h>

static void signal_empty_handler(int signum)
{
    /* do nothing */
}

static void signal_hup_handler(int signum)
{
    /* XXX: I would almost think make this rehash the config,
     * but we really don't have a reason to rehash things, I think.
     * Thought needed here before development.
     */
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

static void signal_term_handler(int signum)
{
    /* Do something later */
}

static void signal_usr1_handler(int signum)
{
    sigyn_fatal("Out of memory!");
}

void signals_init(void)
{
    mowgli_signal_install_handler(SIGHUP, signal_hup_handler);
    mowgli_signal_install_handler(SIGINT, signal_int_handler);
    mowgli_signal_install_handler(SIGTERM, signal_term_handler);

    mowgli_signal_install_handler(SIGUSR1, signal_usr1_handler);
    mowgli_signal_install_handler(SIGSEGV, signal_segv_handler);
}

