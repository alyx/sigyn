#include <stdio.h>
#include "atheme_string.h"

int main(void)
{
    char moo[1024];
    strlcpy(moo, "moo\n", sizeof(moo));
    strlcat(moo, "fish\n", sizeof(moo));
    printf("%s", moo);
    return 0;

}
