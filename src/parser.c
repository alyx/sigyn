/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

irc_user_t *parse_user(char hostmask[])
{
    char *save, *token, *tmp;
    irc_user_t *user;
    
    user = mowgli_alloc(sizeof(irc_user_t));

    if (hostmask == NULL)
        return NULL;

    tmp = strdup(hostmask);

    if (strchr(tmp, '!') != NULL)
    {
        /* Origin is a user. */
        user->server = false;

        token = strtok_r(tmp, "!", &save);
        if (token != NULL)
            user->nick = token;
        token = strtok_r(NULL, "@", &save);
        if (token != NULL)
            user->user = token;
        user->host = save;
    }
    else
    {
        /* Origin is a server. */
        user->nick = tmp;
        user->server = true;
    }
    return user;
}

void preparse(char line[])
{
    char *token, *save;

    me.stats.inB += strlen(line);

    if ((*line == '\n') || (*line == '\000'))
        return;

    save = strdup(line);

    if (strchr(save, '\n') == NULL)
    {
        strip(save, "\r");
        recvq_add(me.uplink.sock, save, false);
    }
    else
    {
        while ((token = strtok_r(NULL, "\n", &save)) && (token != NULL))
        {
            strip(token, "\r");
            recvq_add(me.uplink.sock, token, true);

            if (strchr(save, '\n') == NULL)
            {
                strip(save, "\r");
                recvq_add(me.uplink.sock, save, false);
                break;
            }
        }
    }
}


irc_event_t *parse(char line[])
{
    char *string = strdup(line);
    char *token;
    irc_event_t *event = mowgli_alloc(sizeof(irc_event_t));
    if (string == NULL)
        return NULL;

    strip(string, "\r\n");
    logger(LOG_RAW, ">> %s", string);

    token = strtok(string, " ");
    if (token == NULL)
        return NULL;

    if((strncmp(token, ":", 1)) == 0)
    {
        event->origin = mowgli_alloc(sizeof(irc_user_t));
        event->origin = parse_user(token + 1);
    }
    else
    {
        event->command = token;
    }

    if(token != NULL)
    {
        if(event->command != NULL)
        {
            token = strtok(NULL, "\0");
            event->data = token;
        }
        else
        {
            token = strtok(NULL, " ");
            event->command = token;
        }
    }

    if(token != NULL)
    {
        token = strtok(NULL, " ");

        if (token != NULL)
        {

            if ((strncmp(token, ":", 1)) == 0)
                event->target = token + 1;
            else
                event->target = token;

            if ((strcmp(event->target, me.client->nick)) == 0)
                event->target = event->origin->nick;
        }
    }

    if ((token != NULL) && (!event->data))
    {
        token = strtok(NULL, "\0");
        if (token != NULL)
        {
            if ((strncmp(token, ":", 1)) == 0)
                event->data = token + 1;
            else
                event->data = token;
        }
    }

    if (event != NULL)
    {
        mowgli_hook_call(event->command, event);
    }
    return event;
}
