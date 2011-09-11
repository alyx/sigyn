/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

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
#ifdef _WIN32
    me.uplink.winsock = false;
#endif
    mowgli_init();

    mowgli_hook_init();
    modules_init();
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

int main(int argc, char *argv[])
{
    char hostname[256];
    sigyn_hostname(hostname, 255);
    module_t *m;

    initialise_sigyn(SIGYN_NICK, SIGYN_NICK, SIGYN_REALNAME, UPLINK_SERVER, UPLINK_PORT);
    uplink_connect(me.uplink.hostname, me.uplink.port);

    irc_nick(me.client->nick);
    irc_user(me.client->nick, hostname, me.uplink.hostname, me.client->gecos);

    char text[513];
    int status;
    irc_event_t *event = mowgli_alloc(sizeof(irc_event_t));
    m = module_load("/home/alyx/Projects/sigyn/build/moo");
    if (m == NULL)
        fprintf(stderr, "moo what?\n");
    while (1)
    {
        status = recv(me.uplink.sock, text, 512, 0);
        if (status == 0)
        {
            logger(LOG_STATUS, "The server closed the connection.");
            break;
        }
        if (status == 1)
        {
            logger(LOG_STATUS, "An error has occured reading from the socket: %i", ERRNO);
            break;
        }

        strip(text);
        logger(LOG_RAW, ">> %s", text);
        //XXX: Redo how we get the contents, probably do some shiny select() wrapper.
        event = parse(text);

        if (event->command != NULL)
            fprintf(stderr, "Got %s\n", event->command);

        mowgli_hook_call(event->command, event);

    }
    sigyn_cleanup();
    return 0;
}
