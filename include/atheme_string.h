/*
 * Copyright (c) 2005 Atheme Development Group
 * Rights to this code are as documented in doc/LICENSE.
 *
 * String stuff.
 *
 */

#ifndef __CLAROSTRING
#define __CLAROSTRING

#include <sys/types.h>
#include "config.h"

#ifndef HAVE_STRLCAT
    size_t strlcat(char *dest, const char *src, size_t count);
#endif
#ifndef HAVE_STRLCPY
    size_t strlcpy(char *dest, const char *src, size_t count);
#endif

    void strip(char *line);

#endif

/* vim:cinoptions=>s,e0,n0,f0,{0,}0,^0,=s,ps,t0,c3,+s,(2s,us,)20,*30,gs,hs
 * vim:ts=8
 * vim:sw=8
 * vim:noexpandtab
 */
