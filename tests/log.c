#include "test.h"
#include "logger.h"

int test_logger(char *logstring)
{
    logger_init(LOGFILE);
    logger(LOG_GENERAL, logstring);
    logger_deinit();
    return 1;
}
