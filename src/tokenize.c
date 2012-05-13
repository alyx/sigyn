/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

int tokenize(char *message, char **parv)
{
    int i;
    char *token, *save;

    i = 0;
    save = mowgli_strdup(message);

    while ((i <= MAXPARC) && (token = strtok_r(NULL, " ", &save)) &&
            (token != NULL))
    {
        parv[i] = token;
        i++;
        if (save == NULL || (strlen(save)) < 1)
            break;
    }
    if ((i > MAXPARC) && (save != NULL))
        parv[i] = mowgli_strdup(save);

    if (parv[i] == NULL)
        i--;

    logger(LOG_DEBUG, "[tokenize] parc size: %i", i);

    return i;
}
