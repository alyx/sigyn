/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

fd_set readfds, writefds, nullfds;

/*
 * Routine Description:
 * This routine sets up the data inside the me struct, used continuously
 * throughout Sigyn's runtime.
 *
 * Arguments:
 *     nick   - A string containing the nickname Sigyn should use
 *              to connect with (Sent in the first NICK command).
 *     ident  - A string containing the username Sigyn should use
 *              (Sent in USER).
 *     gecos  - A string containing the "real name" Sigyn should use
 *              (Sent in USER).
 *     uplink - A string containing the hostname Sigyn should connect to.
 *     port   - An integer specifying the port number of the IRC server.
 *
 * Return value:
 *    None.
 */

static void initialise_sigyn(char *nick, char *ident, char *gecos, char *uplink, int port)
{
    me.client = mowgli_alloc(sizeof(irc_user_t));
    me.stats.start = time(NULL);
    me.stats.inB = 0;
    me.stats.outB = 0;
    me.client->nick = nick;
    me.client->user = ident;
    me.client->gecos = gecos;
    me.uplink.port = port;
    me.uplink.hostname = uplink;
    me.uplink.connected = false;
    me.maxfd = 3;
#ifdef _WIN32
    me.uplink.winsock = false;
#endif

    mowgli_init();
    mowgli_hook_init();
    modules_init();
    queue_init();
}

/*
 * Routine Description:
 * This routine provides a method for shutdown-time cleaning of opened files,
 * sockets, et cetera.
 *
 * Arguments:
 *     None
 *
 * Return value:
 *     None
 */

void sigyn_cleanup(void)
{
    logger(LOG_STATUS, "Running cleanup.");
    uplink_disconnect();
#ifdef _WIN32
    if(me.uplink.winsock == true)
    {
        WSACleanup();
        logger(LOG_STATUS, "Shut down winsock.");
    }
#endif
    logger_deinit();
}

/*
 * Routine Description:
 * This routine provides a method for handling fatal errors
 * caused by other routines.
 *
 * Arguments:
 *     format - A string containing the format for the log message.
 *     ...    - An undefined amount of other arguments to be inserted
 *              into the format string.
 *
 * Return value:
 *     None
 */

void sigyn_fatal(char *format, ...)
{
    char buf[BUFSIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, BUFSIZE, format, args);
    va_end(args);
    logger(LOG_FATAL, buf);
    sigyn_cleanup();
    exit(1);
}

void io_loop(void)
{
    int sr;
    struct timeval to;
    static char buffer[BUFSIZE + 1];
    while (1)
    {
        to.tv_usec = 0;
        to.tv_sec  = 1;

        memset(buffer, '\0', BUFSIZE + 1);
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);

        if (((!me.uplink.connected) && (me.uplink.sock != -1)) || (sendq.count != 0))
        {
            FD_SET(me.uplink.sock, &writefds);
            printf("Set socket %d as writable.\n", me.uplink.sock);
        }
        else if (me.uplink.sock != -1)
        {
            FD_SET(me.uplink.sock, &readfds);
            printf("No writing needed. Setting %d as readable.\n", me.uplink.sock);
        }

        if (me.uplink.sock == -1)
            sigyn_fatal("Socked dead. Dying.");

        if ((sr = select(me.maxfd + 1, &readfds, &writefds, &nullfds, &to)) > 0)
        {
            if (FD_ISSET(me.uplink.sock, &writefds))
            {
                if (!me.uplink.connected)
                {
                    sigyn_introduce_client(me.client->nick);
                    sendq_dump(me.uplink.sock);
                    me.uplink.connected = true;
                }
                else
                {
                    sendq_dump(me.uplink.sock);
                }
            }

            if(!(irc_read(me.uplink.sock, buffer)))
                sigyn_fatal("Lost connection to uplink.");

            preparse(buffer);
            recvq_dump(me.uplink.sock);
        }
        else
        {
            if ((sr == -1) && errno != EINTR)
                sigyn_fatal("Lost connection to server: %s", strerror(errno));
        }
    }
}

int main(int argc, char *argv[])
{
    module_t *m;

    initialise_sigyn(SIGYN_NICK, SIGYN_NICK, SIGYN_REALNAME, UPLINK_SERVER, UPLINK_PORT);
    me.uplink.sock = uplink_connect(me.uplink.hostname, me.uplink.port, NULL);
    printf("%d\n", me.uplink.sock);

    m = module_load("/home/alyx/Projects/sigyn/build/moo");
    if (m == NULL)
        fprintf(stderr, "moo what?\n");

    io_loop();

    sigyn_cleanup();
    return 0;
}

