#import "logger.h"
#import "test.h"

void test_logger(void)
{
    logger_init(LOGFILE);
    logger(LOG_RAW, "Example RAW here");
    logger(LOG_GENERAL, "Example GENERAL here");
}
