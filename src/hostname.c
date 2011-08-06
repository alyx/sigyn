/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 *
 * Abstract: This module provides an OS-agnostic function for finding the system hostname.
 */

#ifdef _WIN32
#   include <winsock2.h>
#   pragma comment(lib, "Ws2_32.lib")
#else
#   include <unistd.h>
#endif

char *sigyn_hostname(void)
{
    char hostname[256];
#ifdef _WIN32
    WSADATA wsaData;
    int wsres WSAStartup(MAKEWORD(2, 2), &wsaData);


#endif
    gethostname(hostname, 255);
#ifdef _WIN32
    WSACleanup();
#endif
    return hostname;
}
