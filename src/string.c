/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 *
 * Abstract: This module implements various useful string-related functions.
 */

#include "sigyn.h"

/* copied from atheme */
void strcasecanon(char *str)                                                                                                                                                                    
{
    while (*str)
    {
        *str = toupper(*str);
        str++;
    }
    return;
}


void rmchar(char *line, char *strippers)
{
    int i = 0;
    int position = 0;
    while (strippers[i] != '\0')
    {
        while (line[position] !='\0')
        {
            /*if (strcmp(line[position], strippers[i]))*/
            if (line[position] == strippers[i])
            {
                while (line[position] != '\0')
                {
                    line[position] = line[position + 1];
                    position++;
                }
            }
            else
                position++;
        }
        i++;
        position = 0;
    }
}

void strip(char *line, char *strippers)
{
    int i = 0;
    int position = 0;
    while (strippers[i] != '\0')
    {
        while (line[position] !='\0')
        {
            if (line[position] == strippers[i])
            {
                line[position] = '\0';
            }
            else
                position++;
        }
        i++;
        position = 0;
    }
}

/*
 * Routine Description:
 * Duplicates a string, stripping control codes in the
 * process.
 *
 * Inputs:
 *       - buffer to strip
 *
 * Outputs:
 *       - duplicated buffer without control codes
 *
 * Side Effects:
 *       - none
 */
// FIXME: This destroys strings! make it /STRIP/ the chars, not remove them!
const char *strip_colour_codes(const char *buf)
{
    static char outbuf[BUFSIZE];
    const char *in = buf;
    char *out = outbuf;

    for (; *in != '\0'; in++)
    {
        if (*in > 31)
        {
            *out++ = *in;
            continue;
        }
        else if (*in == 3)
        {
            in++;
            while (isdigit(*in))
                in++;
        }
    }

    *out = '\0';
    return outbuf;
}
