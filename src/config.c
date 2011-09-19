/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#include "sigyn.h"

/*
 * Routine Description:
 * This routine retrieves the value of the section/key requested.
 *
 * Arguments:
 *     section - A string stating the section of the config to locate the key.
 *     key     - A string stating the name of the key to retrieve the value of.
 *
 * Return value:
 *     buf - A string containing the value of the key,
 *           this value is NULL if the key is not found.
 *
 */

char *config_get_string(const char *section, const char *key)
{
    char *buf;
    buf = mowgli_alloc(128);
    ini_gets(section, key, NULL, buf, 128, me.config);
    return buf;
}

/*
 * Routine Description:
 * This routine retrieves the true/false value of the section/key requested.
 *
 * Arguments:
 *     section - A string stating the section of the config to locate the key.
 *     key     - A string stating the name of the key to retrieve the value of.
 *
 * Return value:
 *     This function returns a bool, defaulting to false if the key is
 *     not located.
 *
 */

bool config_get_bool(const char *section, const char *key)
{
    return ini_getbool(section, key, 0, me.config);
}

/*
 * Routine Description:
 * This routine retrieves the numeric value of the section/key requested.
 *
 * Arguments:
 *     section - A string stating the section of the config to locate the key.
 *     key     - A string stating the name of the key to retrieve the value of.
 *
 * Return value:
 *     i - An integer containing the numeric value of the section/key,
 *         defaulting to 0 if the key is not found.
 *
 */

int config_get_int(const char *section, const char *key)
{
    int i;
    char buf[128];

    ini_gets(section, key, NULL, buf, 128, me.config);
    if (buf == NULL)
        return 0;
    i = atoi(buf);
   
    return i;
}
