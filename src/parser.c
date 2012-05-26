/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

static void parse_isupport(char * string)
{
    /*char * save, * token, * tmp, * ptr;*/
    char * tmp, * save, * token, * ptr;
    unsigned int * i;

    i = mowgli_alloc(1);
    *i = 1;

    tmp = mowgli_strdup(string);
    ptr = strrchr(tmp, ':');
    *ptr = '\0';
    ptr = NULL;

    token = strtok_r(tmp, " ", &save);

    while (token != NULL)
    {
        ptr = strchr(token, '=');
        if (ptr == NULL)
            mowgli_patricia_add(isupport_table, token, i);
        else
        {
            *ptr = '\0';
            mowgli_patricia_add(isupport_table, token, mowgli_strdup(ptr+1));
        }
        token = strtok_r(NULL, " ", &save);
    }
    mowgli_free(tmp);
}

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

    if (hostmask == NULL)
        return NULL;

    user = mowgli_alloc(sizeof(irc_user_t));

    tmp = mowgli_strdup(hostmask);
    user->hostmask = mowgli_strdup(hostmask);

    if (strchr(tmp, '!') != NULL)
    {
        /* Origin is a user. */
        user->server = false;

        token = strtok_r(tmp, "!", &save);
        if (token != NULL)
            user->nick = mowgli_strdup(token);
        token = strtok_r(NULL, "@", &save);
        if (token != NULL)
            user->user = mowgli_strdup(token);
        user->host = mowgli_strdup(save);
    }
    else
    {
        /* Origin is a server. */
        user->nick = mowgli_strdup(tmp);
        user->server = true;
    }

    mowgli_free(tmp);

    return user;
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

void parse(char line[])
{
    char *string = mowgli_strdup(line);
    char *token;
    irc_event_t *event = mowgli_alloc(sizeof(irc_event_t));
    if (string == NULL)
        return;

    me.stats.inB += strlen(string);

    strip(string, "\r\n");
    logger(LOG_RAW, ">> %s", string);

    token = strtok(string, " ");
    if (token == NULL)
        return;

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
        if (!strcmp(event->command, "005"))
            parse_isupport(event->data);

        mowgli_hook_call(event->command, event);
    }

    mowgli_free(string);
    if (event->origin)
    {
        mowgli_free(event->origin->hostmask);

        if (event->origin->server)
            mowgli_free(event->origin->nick);
        else
        {
            mowgli_free(event->origin->nick);
            mowgli_free(event->origin->user);
            mowgli_free(event->origin->host);
        }
        
        mowgli_free(event->origin);
    }
    mowgli_free(event);
}
