#include "sigyn.h"
#include "test.h"

int test_hostname(void)
{
    char host[256];
    sigyn_hostname(host);
    if (host = NULL):
        return 0;
    return 1;
}
