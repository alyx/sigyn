#include "sigyn.h"

bool glob(const char * match, const char * string)
{
    if (match == NULL || string == NULL)
        return false;
    if (!strcmp(match, "*")
            || (!strcmp(match,"?") && strlen(string) == 1))
        return true;

    while (1)
    {
        if (*match == '*')
        {
            while (*match == '*')
                match++;

            while (*match == '?' && *string)
            {
                match++;
                string++;
            }

            if (*match == '\0')
                return true;

            if (*match == '*')
                return glob(match, string);

            while (*string)
            {
                if ((toupper(*string) ==
                            toupper(*match))
                        && glob(match+1, string+1))
                    return true;

                string++;
            }
        }

        if (*string == '\0')
            return false;

        if (*match != '?')
            if ((toupper(*string)) != toupper(*match))
                return false;

        string++;
        match++;
    }
}
