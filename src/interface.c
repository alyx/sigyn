/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alexandria Wolcott <alyx@malkier.net>
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

void command_fail(int reason, irc_user_t *origin, const char *command)
{
    command_t *cmd;

    cmd = command_find(command);

    switch(reason)
    {
        case CMD_BADPARAM:
            irc_notice(origin->nick, "Incorrect parameters for \2%s\2", cmd->name);
            if (cmd->syntax != NULL)
                irc_notice(origin->nick, "Syntax: %s", cmd->syntax);
            break;

        case CMD_NEEDSPARAM:
            irc_notice(origin->nick, "Insufficient parameters for \2%s\2", cmd->name);
            if (cmd->syntax != NULL)
                irc_notice(origin->nick, "Syntax: %s", cmd->syntax);
            break;
        
        case CMD_NOAUTH:
            irc_notice(origin->nick, "You are not authorised to use \2%s\2", cmd->name);
            break;

        default:
            irc_notice(origin->nick,
                       "An unknown error occured while executing \2%s\2. "
                       "Please try again later.",
                       cmd->name);
            break;
    }
}

void command_reply(const char * target, char * fmt, ...)
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
