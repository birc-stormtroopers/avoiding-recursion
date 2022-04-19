#include "traversal.h"
#include <stdio.h>
#include <stdlib.h>

// clang-format off
enum op { TRAVERSE, EMIT };
typedef struct frame
{
    enum op op;
    tree t;
} frame;
// clang-format on

static void push_frame(stack *stack, enum op op, tree t)
{
    // Only push non-empty trees
    if (t)
    {
        push(stack, &(frame){.op = op, .t = t});
    }
}

dynarr stack_traversal(tree t)
{
    dynarr a = new_dynarr();
    stack stack = new_stack(sizeof(frame));
    push_frame(&stack, TRAVERSE, t);
    while (!is_empty(&stack))
    {
        frame frame = *(struct frame *)pop(&stack);
        switch (frame.op)
        {
        case TRAVERSE:
            push_frame(&stack, TRAVERSE, frame.t->right);
            push_frame(&stack, EMIT, frame.t);
            push_frame(&stack, TRAVERSE, frame.t->left);
            break;

        case EMIT:
            append(&a, frame.t->value);
            break;
        }
    }

    return a;
}
