/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

/*
 * Routine Description:
 * This routine creates a method for informing users that their command failed
 * using a variety of predefined templates.
 *
 * Arguments:
 *     reason  - An integer stating the reason (See interface.h for an enum
 *               containing named values for this).
 *     origin  - An irc_user_t object stating the command origin.
 *     command - A string specifying the command name.
 *
 */

void command_fail(const int reason,
                  const irc_user_t *origin,
                  const char *command)
{
    command_t *cmd = command_find(command);

    switch (reason)
    {
    case CMD_BADPARAM:
        irc_notice(origin->nick,
                   "Incorrect parameters for \2%s\2",
                   cmd->name);

        if (cmd->syntax != NULL)
            irc_notice(origin->nick, "Syntax: %s", cmd->syntax);

        break;

    case CMD_NEEDSPARAM:
        irc_notice(origin->nick,
                   "Insufficient parameters for \2%s\2",
                   cmd->name);

        if (cmd->syntax != NULL)
            irc_notice(origin->nick, "Syntax: %s", cmd->syntax);

        break;

    case CMD_NOAUTH:
        irc_notice(origin->nick,
                   "You are not authorised to use \2%s\2",
                   cmd->name);

        break;

    default:
        irc_notice(origin->nick,
                   "An unknown error occured while executing \2%s\2. "
                   "Please try again later.",
                   cmd->name);
        break;
    }
}

/*
 * Routine Description:
 * This routine creates a method for successfully responding to a user's command.
 * This routine implements logic to appropriately respond via either PRIVMSG or
 * NOTICE, depending on target type.
 *
 * Arguments:
 *     target  - A string containing the target for this message.
 *     fmt     - A string containing a printf-style string to be sent.
 *     ...     - Any additional parameters for fmt.
 *
 */

void command_reply(const char *target, char *fmt, ...)
{
    char buf[BUFSIZE];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buf, BUFSIZE, fmt, args);
    va_end(args);

    if (ischannel(target))
        irc_privmsg(target, buf);
    else
        irc_notice(target, buf);
}

/*
 * Routine Description:
 * This routine creates a method for successfully responding to a user's command.
 * This routine implements logic to appropriately respond via either PRIVMSG or
 * NOTICE, depending on target type.
 * 
 * The _safe function is specifically for use in times when the fmt content is
 * retrieved from another source or considered 'unsafe'. It will automatically
 * substitute all newlines ('\n') with an additional PRIVMSG.
 * 
 * This result in an fmt of "hello\nNICK world" being "PRIVMSG #channel :hello",
 * "PRIVMSG #channel :NICK world", rather than "PRIVMSG #channel :hello", "NICK world".
 *
 * Arguments:
 *     target  - A string containing the target for this message.
 *     fmt     - A string containing a printf-style string to be sent.
 *     ...     - Any additional parameters for fmt.
 *
 */

void command_reply_safe(const char *target, char *fmt, ...)
{
    char buf[BUFSIZE * 4], *arr[BUFSIZE], *ptr;
    int i, j;
    va_list args;
    void (*reply)(const char*, const char*, ...);

    va_start(args, fmt);
    vsnprintf(buf, BUFSIZE * 4, fmt, args);
    va_end(args);

    if (ischannel(target))
        reply = irc_privmsg;
    else
        reply = irc_notice;

    i = 0;
    ptr = strtok(buf, "\n");

    if (ptr == NULL)
    {
        command_reply(target, buf);
        return;
    }

    while (ptr != NULL)
    {
        arr[i] = ptr;
        i++;
        ptr = strtok(NULL, "\n");
    }

    for (j = 0; j < i; j++)
    {
        reply(target, arr[j]);
    }
}