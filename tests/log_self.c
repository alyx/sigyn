/*#include "test.h"*/
#include "logger.h"

int main(void)
{
    logger_init(LOGFILE);
    logger(LOG_RAW, "Example RAW here");
    logger(LOG_GENERAL, "Example GENERAL here");
    return 0;
}

