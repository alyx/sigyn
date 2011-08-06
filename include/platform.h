#ifndef __SIGYN_PLATFORM_H
#define __SIGYN_PLATFORM_H

#ifdef _WIN32
#   include <winsock2.h>
#   define ERRNO WSAGetLastError()
#   define errno GetLastError()
#else
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <netdb.h>
#   define ERRNO errno
#endif

#endif // __SIGYN_PLATFORM_H
