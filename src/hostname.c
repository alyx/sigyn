/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 *
 * Abstract: This module provides an OS-agnostic function for finding the system hostname.
 */

#define __USE_BSD

#ifdef _WIN32
#   include <winsock2.h>
#   pragma comment(lib, "Ws2_32.lib")
#else
#   include <unistd.h>
    extern int gethostname (char *__name, size_t __len) __THROW __nonnull ((1));
#endif

/*
 * Routine Description:
 * This routine provides an operating-system agnostic method of discovering
 * the hostname of the system.
 *
 * Arguments:
 *     host - A pointer to a string filled with the hostname by gethostname().
 *     len  - The maximum length we allow the hostname to be.
 *
 * Return value:
 *     res - sigyn_hostname() forwards the return value of gethostname().
 *           This is 0 for success, and -1 for failure.
 *
 */

int sigyn_hostname(char *host, int len)
{
    int res;
#ifdef _WIN32
    WSADATA wsaData;
    int wsres = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsres != 0)
        return -1;
#endif
    res = gethostname(host, len);
#ifdef _WIN32
    WSACleanup();
#endif
    return res;
}
