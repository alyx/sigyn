#include "sigyn.h"

char *config_get_string(const char *section, const char *key)
{
    char *buf;
    buf = mowgli_alloc(128);
    ini_gets(section, key, NULL, buf, 128, me.config);
    return buf;
}

bool config_get_bool(const char *section, const char *key)
{
    return ini_getbool(section, key, 0, me.config);
}

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
