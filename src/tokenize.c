/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

int tokenize(char *message, char **parv)
{
    int i;
    char *token, *save;

    i = 0;
    save = strdup(message);

    while ((i <= MAXPARC) && (token = strtok_r(NULL, " ", &save)) &&
            (token != NULL))
    {
        printf("%d\n", i);
        parv[i] = token;
        i++;
        if ((strlen(save)) < 1)
            break;
    }
    if ((i > MAXPARC) && (save != NULL))
        parv[i] = save;

    if (parv[i] == NULL)
        i--;

    return i;
}
