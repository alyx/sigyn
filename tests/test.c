#include "test.h"

void result(char *name, int result)
{
    printf("Function %s %s\n", name, (result ? "passed" : "failed"));
}

int main(void)
{
    result("test_logger", test_logger("Testing Sigyn logger"));
    result("test_hostname", test_hostname());
    test_parse();
    return 0;
}
