#include "traversal.h"
#include <assert.h>

int main(void)
{
    stack stack = new_stack(sizeof(int));
    push(&stack, &(int){1});
    push(&stack, &(int){2});
    push(&stack, &(int){3});

    assert(3 == *(int *)pop(&stack));
    assert(2 == *(int *)pop(&stack));
    assert(1 == *(int *)pop(&stack));

    return 0;
}