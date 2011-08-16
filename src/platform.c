#include "platform.h"
#include "logger.h"
#include "config.h"

void sigyn_fatal(char *format, ...)
{
    char buf[BUFSIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, BUFSIZE, format, args);
    va_end(args);
    logger(LOG_FATAL, buf);
#ifdef _WIN32
    if (me.uplink.winsock == true)
        WSACleanup();
#endif
    //sigyn_cleanup();
    //XXX: Code a cleanup function.
    exit(1);
}
