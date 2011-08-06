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
//#   include <unistd.h>
#include <unistd.h>
#   error k
#endif

int sigyn_hostname(char *host)
{
    int res;
#ifdef _WIN32
    WSADATA wsaData;
    int wsres WSAStartup(MAKEWORD(2, 2), &wsaData);


#endif
    res = gethostname(host, (sizeof(host) -1));
#ifdef _WIN32
    WSACleanup();
#endif
    return res;
}
