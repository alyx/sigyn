#include <stdio.h>
#include "atheme_string.h"

int main(void)
{
    char moo[1024];
    strlcat(moo, "moo\n", 5);
    printf("%s", moo);
    return 0;

}
