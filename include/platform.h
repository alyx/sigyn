/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) 2011-2012 Alyx Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#ifndef SIGYN_PLATFORM_H
#define SIGYN_PLATFORM_H

/* Create some magic for hiding warnings about unused variables. */
#if defined(GCC) || defined(__clang__) || defined(__ICC)
#   define UNUSED __attribute__((unused))
#else
#   define UNUSED
#endif


/* Handle networking headers. Windows wants Winsock, everyone else uses
 * BSD-style sockets.
 */
#ifdef _WIN32
#   include <winsock2.h>
#   include <ws2tcpip.h>
#else
#   include <sys/socket.h>
#   include <netdb.h>
#   include <arpa/inet.h>
#   include <fcntl.h>
#endif

/* And while we are dealing with networking oddities, create a socket_t
 * data type, so we do not have to have loads of
 * 		#ifdef _WIN32
 * 		int function(SOCKET moo);
 * 		#else
 * 		int function(int moo);
 * 		#endif
 * in our code.
 */
#ifdef _WIN32
	typedef SOCKET socket_t;
#else
	typedef int socket_t;
#endif

/* Windows likes to rename things, and has replaced errno with either
 * GetLastError() or WSAGetLastError(), depending on whether or not
 * you are dealing with socket code. Deal with these here by creating 
 * some macros that everyone can play nicely with.
 */
#ifndef _WIN32
#   define close_portable close
#   define ERRNO errno
#else
#   define snprintf sprintf_s
#   define vsnprintf _vsnprintf
#   define close_portable closesocket
#   define ERRNO WSAGetLastError()
#   define errno GetLastError()
#endif

/* Define the file extension used by the system for dynamically loadable
 * libraries (What Sigyn uses for its module framework).
 */
#if defined(__APPLE__)
#   define PLATFORM_SUFFIX ".impl"
#elif defined(__FreeBSD__) || defined(__unix__) || defined(__unix) || defined(sun)
#   define PLATFORM_SUFFIX ".so"
#elif defined(__hpux)
#   define PLATFORM_SUFFIX ".sl"
#elif defined(_MSC_VER) || defined(_WIN32)
O#   define PLATFORM_SUFFIX ".dll"
#else
#   error Unknown platform - Dynamic library suffix unknown.
#endif

#endif /* SIGYN_PLATFORM_H */
