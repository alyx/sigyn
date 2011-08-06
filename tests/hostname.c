#include "sigyn.h"
#include "test.h"

int test_hostname(void)
{
    char *host = sigyn_hostname();
    if (host = NULL):
        return 0;
    return 1;
}
