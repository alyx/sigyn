/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"
#include "ext/getopt_long.h"

fd_set readfds, writefds, nullfds;
char *config_file;

/*
 * Routine Description:
 * This routine sets up the data inside the me struct, used continuously
 * throughout Sigyn's runtime.
 *
 * Arguments:
 *     nick    - A string containing the nickname Sigyn should use
 *               to connect with (Sent in the first NICK command).
 *     ident   - A string containing the username Sigyn should use
 *               (Sent in USER).
 *     gecos   - A string containing the "real name" Sigyn should use
 *               (Sent in USER).
 *     uplink  - A string containing the hostname Sigyn should connect to.
 *     port    - An integer specifying the port number of the IRC server.
 *     use_ssl - A boolean specifying whether to use SSL or not.
 *     vhost   - A string containing the hostname for Sigyn to bind to (Optional). 
 *
 * Return value:
 *    None.
 */

void initialise_sigyn(char *nick, char *ident, char *gecos, char *uplink, 
        char * port, bool use_ssl, char *vhost)
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
    me.uplink.ssl = use_ssl;
    me.uplink.vhost = vhost;
    me.uplink.connected = false;
    me.maxfd = 3;

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

    while ((r = mowgli_getopt_long(argc, argv, "c:hv", long_opts, NULL)) != -1)
    {
        switch (r)
        {
	        case 'h':
                printf("usage: sigyn [-hv] [-c config]\n\n"
                " -c <path>   Specify a configuration file for sigyn to use\n"
                " -h          Print this message and exit\n"
                " -v          Print the version information and exit\n");
                exit(EXIT_SUCCESS);
                break;
            case 'v':
                printf("%s by Alyx Wolcott.\n", PACKAGE_STRING);
                exit(EXIT_SUCCESS);
                break;
            case 'c':
                config_file = mowgli_strdup(mowgli_optarg);
                break;
            default:
                printf("\nusage: sigyn [-hv] [-c config]\n");
                exit(EXIT_SUCCESS);
                break;
        }
    }
    if (config_file == NULL)
        config_file = mowgli_strdup(SYSCONFDIR "/sigyn.conf");
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
    if (me.uplink.connected)
        mowgli_linebuf_destroy(me.uplink.line);
    mowgli_eventloop_destroy(me.ev);
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
    if (me.uplink.connected)
        irc_quit(buf);
    sigyn_cleanup();
    exit(1);
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
    me.ev = mowgli_eventloop_create();

    signals_init();

    parse_commandline_options(argc, argv);

    me.config = mowgli_config_file_load(config_file);

    if(me.config == NULL)
        sigyn_fatal("Cannot load configuration file.");
    logger_init(me.config->entries);
    config_check(me.config);

    me.uplink.line = new_conn(me.uplink.hostname, me.uplink.port, me.uplink.ssl, read_irc, NULL);
    if (me.uplink.line == NULL)
        sigyn_fatal("Connection to uplink failed.");
    me.uplink.connected = true;

    loadmodules(me.config->entries);

    sigyn_introduce_client(me.client->nick);
    mowgli_eventloop_run(me.ev);

    sigyn_cleanup();
    return EXIT_SUCCESS;
}

