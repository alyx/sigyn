/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

#if 0
static irc_user_t *parse_user(char *hostmask)
{
    irc_user_t *user = mowgli_alloc(sizeof(irc_user_t));
    char *ptr;

    if((ptr = strchr(hostmask, '!')) != NULL)
    {
        ptr = '\0';
        user->nick = hostmask;
        hostmask = ++ptr;
    }
    if ((ptr = strchr(hostmask, '@')) != NULL)
    {
        ptr = '\0';
        user->user = hostmask;
        hostmask = ++ptr;
    }

    user->host = hostmask;

    return user;
}
#endif

void preparse(char line[])
{
    char *token, *save;

    me.stats.inB += strlen(line);

    if ((*line == '\n') || (*line == '\000'))
        return;

    save = strdup(line);
    
    if (strchr(save, '\n') == NULL)
        goto partial;

    while ((token = strtok_r(NULL, "\n", &save)) && (token != NULL))
    {
        recvq_add(me.uplink.sock, token, true);
        
        if (strchr(save, '\n') == NULL)
            break;
    }
    
    partial:
        token = save;
        recvq_add(me.uplink.sock, token, false);
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
    if((strncmp(token, ":", 1)) == 0)
    {
       event->origin = token + 1;
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
        if (event->data != NULL)
            return event;

        token = strtok(NULL, "\0");
        event->data = token;
    }

    if (event != NULL)
    {
        printf("\tGot %s\n", event->command);
        mowgli_hook_call(event->command, event);
    }
    return event;
}
