/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

/*
 * Routine Description:
 * This function parses a given hostmask and returns an irc_user_t object
 * containing the results.
 *
 * Arguments:
 *     hostmask - A string containing the IRC hostmask.
 *
 * Return value:
 *     user - An irc_user_t object containing the data from 
 *            the parsed hostmask.
 *            
 */

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
            user->nick = strdup(token);
        token = strtok_r(NULL, "@", &save);
        if (token != NULL)
            user->user = strdup(token);
        user->host = save;
    }
    else
    {
        /* Origin is a server. */
        user->nick = strdup(tmp);
        user->server = true;
    }

    free(tmp);

    return user;
}

/*
 * Routine Description:
 * This routine converts a raw string from the server into a format suitable for placing into the recieve queue.
 *
 * Arguments:
 *     line - A string containing the raw IRC line.
 *
 * Return value:
 *     None
 *
 */

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

/*
 * Routine Description:
 * This routine parses a raw IRC line into an irc_event_t object.
 *
 * Arguments:
 *     line - A string containing the raw IRC line.
 *
 * Return value:
 *     event - An irc_event_t object containing the parsed string data.
 *
 */

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

    free(string);

    return event;
}
