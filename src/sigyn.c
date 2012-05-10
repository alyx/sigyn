/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"
#include "ext/getopt_long.h"

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

void initialise_sigyn(char *nick, char *ident, char *gecos, char *uplink, 
        uint16_t port)
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

    mowgli_hook_bootstrap();
    modules_init();
    queue_init();
    command_init();
    channel_init();
    timer_init();
}

/*
 * Routine Description:
 * This routine provides a way of parsing the provided command line
 * arguments to the program.
 *
 * Arguments:
 *     Number of arguments - An integer containing the number of arguments
 *			     for the character array.
 *     Arguments - A character array containing all the command line options.
 *
 * Return value:
 *     None
 */

void parse_commandline_options(int argc, char **argv)
{
    int r;

    mowgli_getopt_option_t long_opts[] = {
      { NULL, 0, NULL, 0, 0 },
    };

    while ((r = mowgli_getopt_long(argc, argv, "hv", long_opts, NULL)) != -1)
    {
	switch (r)
	{
	    case 'h':
		printf("usage: sigyn [-hv]\n\n"
		  " -h		Print this message and exit\n"
		  " -v		Print the version information and exit\n");
		exit(EXIT_SUCCESS);
		break;
	    case 'v':
		printf("%s by Alexandria Wolcott.\n", PACKAGE_STRING);
		exit(EXIT_SUCCESS);
		break;
	    default:
		printf("\nusage: sigyn [-hv]\n");
		exit(EXIT_SUCCESS);
		break;
	}
    }
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
    logger(LOG_GENERAL, "Running cleanup.");
    modules_shutdown();
    uplink_disconnect();
#ifdef _WIN32
    if(me.uplink.winsock == true)
    {
        WSACleanup();
        logger(LOG_GENERAL, "Shut down winsock.");
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
    logger(LOG_ERROR, buf);
    irc_quit(buf);
    sigyn_cleanup();
    exit(1);
}

static void io_loop(void)
{
    int sr;
    struct timeval to;
    time_t currtime, next_event;
    static char buffer[BUFSIZE + 1];
    
    to.tv_usec = 0;
    
    while (1)
    {
        currtime = time(NULL);
        next_event = get_next_timer();

        if (next_event <= currtime)
        {
            run_timers(currtime);
        }
        
        if (next_event > currtime)
            to.tv_sec = (next_event - currtime);
        else
            to.tv_sec  = 1;

        memset(buffer, '\0', BUFSIZE + 1);
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);

        if (((!me.uplink.connected) && (me.uplink.sock != -1)) || (sendq.count != 0))
        {
            FD_SET(me.uplink.sock, &writefds);
        }
        else if (me.uplink.sock != -1)
        {
            FD_SET(me.uplink.sock, &readfds);
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

            if(!(read_irc(me.uplink.sock, buffer)))
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


static void loadmodules(mowgli_config_file_entry_t * entry)
{
    module_t * m;

    while (entry != NULL)
    {
        logger(LOG_DEBUG, "loadmodules(): Entry name, %s", entry->varname);
        if (!strcmp(entry->varname, "loadmodule"))
        {
            m = module_load(entry->vardata);
            if (m != NULL)
                logger(LOG_GENERAL, "[Modules] Loaded module %s\n", m->name);
            else
                logger(LOG_GENERAL, "[Modules] Failed to load module %s\n", entry->vardata);
        }
        if (entry->entries != NULL)
            loadmodules(entry->entries);

        entry = entry->next;
    }
}

int main(int argc, char *argv[])
{
    char config[BUFSIZE];

    signals_init();
    parse_commandline_options(argc, argv);

    snprintf(config, BUFSIZE, "%s/%s", SYSCONFDIR, "sigyn.conf");

    me.config = mowgli_config_file_load(config);

    if(me.config == NULL)
        sigyn_fatal("Cannot load configuration file.");
    logger_init(me.config->entries);
    config_check(me.config);

    me.uplink.sock = uplink_connect(me.uplink.hostname, me.uplink.port, NULL);

    loadmodules(me.config->entries);
    io_loop();

    sigyn_cleanup();
    return EXIT_SUCCESS;
}

