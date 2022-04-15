#include "traversal.h"
#include <assert.h>
#include <stdio.h>

int main(void)
{
    dynarr a = new_dynarr();
    for (int i = 0; i < 10; i++)
    {
        append(&a, i);
    }
    for (int i = 0; i < 10; i++)
    {
        assert(i == a->data[i]);
    }
    free(a);
    return 0;
}
