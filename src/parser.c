/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

irc_event_t *parse(char *text)
{
    irc_event_t *event = mowgli_alloc(sizeof(irc_event_t));
    char *ptr = text;
    char *user = NULL, *ptr, *cmd, *data = NULL;

    if (text != NULL)
    {
        if (*text == '\n')
            return;
        if (*text == '\0')
            return;

        char *logmsg = snprintf(">> %s", text);
        logger(LOG_RAW, logmsg);

            if (line[0] == ':') {
                user = &text[1];
                ptr = strchr(user, ' ');
                if (p == NULL)
                    return -1;
                *ptr++ = '\0';
                cmd = ptr;
                if ((ptr = strchr(user, ' ')))
                    *ptr = '\0';
            } else
                cmd = line;
            ptr = strchr(cmd, ' ');
            if (ptr) {
                *ptr++ = '\0';
                data = ptr;
                if (*data == ':')
                    data++;
            }

            if (user != NULL)
            {
                irc_user_t *origin = parse_user(user);

                /* If the origin is a server */
                if (!origin.nick)
                    origin.nick = origin.host;

                event.origin = origin;
            }

            event.command = cmd;

    }
}

irc_user_t *parse_user(char *hostmask)
{
    irc_user_t *user = mowgli_alloc(sizeof(irc_user_t));
    char *ptr;

    if((ptr = strchr(hostmask, '!')) != NULL)
    {
        *ptr = '\0';
        user.nick = hostmask;
        hostmask = ++ptr;
    }
    if ((ptr = strchr(hostmask, '@')) != NULL)
    {
        *ptr = '\0';
        user.user = hostmask;
        hostmask = ++ptr;
    }

    user.host = host;

    return user;
}
