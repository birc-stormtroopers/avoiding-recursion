#include "traversal.h"
#include <stdio.h>
#include <stdlib.h>

// clang-format off
enum op { TRAVERSE, EMIT };
struct frame { enum op op; tree t; };
// clang-format on

dynarr stack_traversal(tree t)
{
    dynarr a = new_dynarr();
    // clang-format off
    STACK(struct frame) stack = NEW_STACK(struct frame);
    // clang-format on

    PUSH(struct frame, stack, .op = TRAVERSE, .t = t);
    while (!IS_EMPTY(stack))
    {
        struct frame frame = POP(struct frame, stack);
        if (!frame.t)
            continue;
        switch (frame.op)
        {
        case TRAVERSE:
            PUSH(struct frame, stack, .op = TRAVERSE, .t = frame.t->right);
            PUSH(struct frame, stack, .op = EMIT, .t = frame.t);
            PUSH(struct frame, stack, .op = TRAVERSE, .t = frame.t->left);
            break;
        case EMIT:
            append(&a, frame.t->value);
            break;
        }
    }
    FREE_STACK(stack);

    return a;
}
