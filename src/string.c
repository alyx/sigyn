/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 *
 * Abstract: This module implements various useful string-related functions.
 */

#include <string.h>
#include "sigyn.h"

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
const char *strip_colors_codes(const char *buf)
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

/* These functions are taken from Linux. */
size_t strlcat(char *dest, const char *src, size_t count)
{
	size_t dsize = strlen(dest);
	size_t len = strlen(src);
	size_t res = dsize + len;

	dest += dsize;
	count -= dsize;

	if (len >= count)
		len = count - 1;

	memcpy(dest, src, len);

	dest[len] = 0;

	return res;
}

size_t strlcpy(char *dest, const char *src, size_t size)
{
	size_t ret = strlen(src);

	if (size)
	{
		size_t len = (ret >= size) ? size - 1 : ret;
		memcpy(dest, src, len);
		dest[len] = '\0';
	}

	return ret;
}
