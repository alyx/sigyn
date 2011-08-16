#ifndef __SIGYN_PLATFORM_H
#define __SIGYN_PLATFORM_H

#include <stdarg.h>

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

extern void sigyn_fatal(char *format, ...);

#endif // __SIGYN_PLATFORM_H