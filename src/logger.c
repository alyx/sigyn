#include "logger.h"

void logger_init(const char *filename)
{
    if ((logfile = fopen(filename, "a")) == NULL)
    {
        fprintf(stderr, "Cannot open logfile\n");
        exit(1);
    }
}

/*static void va_logger(unsigned int level, const char *format, va_list args)*/
/*{*/
    /*char buf[BUFSIZE];*/
    /*char datetime[64];*/
    /*time_t t;*/
    /*struct tm tm;*/

    /*vsnprintf(buf, BUFSIZE, format, args);*/

    /*time(&t);*/
    /*tm = *localtime(&t);*/
    /*strftime(datetime, sizeof(datetime) - 1, "[%d/%m/%Y %H:%M:%S]", &tm);*/

    /*fprintf(logfile, "%s %s\n", datetime, buf);*/
/*}*/

void logger(unsigned int level, char *format, ...)
{
    char buf[BUFSIZE], datetime[64];
    time_t t;
    struct tm tm;

    va_list args;
    va_start(args, format);
    vsnprintf(buf, BUFSIZE, format, args);
    va_end(args);

    time(&t);
    tm = *localtime(&t);
    strftime(datetime, sizeof(datetime) - 1, "[%d/%m/%Y %H:%M:%S]", &tm);
    fprintf(logfile, "%s %s\n", datetime, buf);
    fflush(logfile);
}
