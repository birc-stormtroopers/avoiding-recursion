#include "traversal.h"
#include <assert.h>

int main(void)
{
    // clang-format off
    STACK(int) stack = NEW_STACK(int);
    // clang-format on

    PUSH(int, stack, 1);
    PUSH(int, stack, 2);
    PUSH(int, stack, 3);

    assert(3 == POP(int, stack));
    assert(2 == POP(int, stack));
    assert(1 == POP(int, stack));

    FREE_STACK(stack);

    return 0;
}
