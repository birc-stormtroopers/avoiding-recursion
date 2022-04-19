#include "traversal.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_STACK_SIZE 8 // Don't(!!!) use zero

struct stack *new_stack(size_t frame_size)
{
    struct stack *stack = malloc(offsetof(struct stack, data) + INIT_STACK_SIZE * frame_size);
    assert(stack);
    stack->cap = INIT_STACK_SIZE;
    stack->len = 0;
    stack->frame_size = frame_size;
    return stack;
}

bool is_empty(struct stack **stack)
{
    return (*stack)->len == 0;
}

void *pop(struct stack **stack)
{
    return &(*stack)->data[--(*stack)->len * (*stack)->frame_size];
}

void push(struct stack **stack, void *frame)
{
    if ((*stack)->len == (*stack)->cap)
    {
        (*stack)->cap *= 2;
        *stack = realloc(*stack,
                         offsetof(struct stack, data) + (*stack)->cap * (*stack)->frame_size);
        assert(*stack);
    }
    memcpy(&(*stack)->data[(*stack)->len++ * (*stack)->frame_size],
           frame, (*stack)->frame_size);
}
