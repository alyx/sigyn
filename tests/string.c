#include <stdio.h>
#include "atheme_string.h"

int main(void)
{
    char moo[1024];
    size_t cpy, cat;
    cpy = strlcpy(moo, "moo\n", sizeof(moo));
    cat = strlcat(moo, "fish\n", sizeof(moo));
    if (cpy == NULL):
        return 0;
    if (cat == NULL):
        return 0;
    printf("%s", moo);
    return 1;
}
