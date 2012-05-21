#include "sigyn.h"

static void parse_isupport(char * string)
{
    char * save, * token, * tmp, * ptr;

    tmp = mowgli_strdup(string);

    token = strtok_r(tmp, " ", &save);

    while (token != NULL)
    {
        if (!strncmp(token, "CASEMAPPING", 11))
        {
            ptr = token + 12;
            me.isupport.casemapping = strdup(tmp);
        }
        else if (!strncmp(token, "NICKLEN", 7))
        {
            ptr = token + 8;
            me.isupport.len.nick = (size_t)strtol(tmp, (char **)NULL, 10);
        }
        else if (!strncmp(token, "CHANNELLEN", 10))
        {
            ptr = token + 11;
            me.isupport.len.channel = (size_t)strtol(tmp, (char **)NULL, 10);
        }
        else if (!strncmp(token, "TOPICLEN", 8))
        {
            ptr = token + 9;
            me.isupport.len.topic = (size_t)strtol(tmp, (char **)NULL, 10);
        }

        token = strtok_r(NULL, " ", &save);
    }

    mowgli_free(tmp);
}

