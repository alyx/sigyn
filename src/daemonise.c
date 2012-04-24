#include "sigyn.h"

void daemonise(const char *pidpath)
{
    FILE *pidfile;
    int i;

    umask(077);
#ifdef HAVE_FORK

    close(0);
    if (open("/dev/null", O_RDWR) != 0)
    {
        logger(LOG_ERROR, "Cannot open /dev/null, forking impossible.");
        return;
    }
    if ((i = fork()) < 0)
    {
        logger(LOG_ERROR, "Unable to fork into background, forking impossible.");
        return;
    }
    else if (i != 0)
    {
        logger(LOG_GENERAL, "pid %d", i);
        logger(LOG_GENERAL, "running in background mode from %s", PREFIX);
        exit(EXIT_SUCCESS);
    }

    if (setsid() < 0)
        sigyn_fatal("unable to create new session: %s", strerror(errno));

    dup2(0,1);
    dup2(0,2);
#else
#ifdef HAVE_GETPID
    logger(LOG_GENERAL, "pid %d", getpid());
#endif
    logger(LOG_GENERAL, "running in foreground mode from %s", PREFIX);
#endif

#ifdef HAVE_GETPID
    if ((pidfile = fopen(pidpath, "w")))
    {
        fprintf(pidfile, "%d\n", getpid());
        fclose(pidfile);
    }
    else
        sigyn_fatal("sigyn: unable to write pid file");
#endif
}
