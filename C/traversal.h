#ifndef TRAVERSAL_H
#define TRAVERSAL_H

#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

// Dynamic array that grows as we append to it.
// This is the analogue to Python lists
typedef struct dynarr_data *dynarr;
struct dynarr_data
{
    size_t len;
    size_t cap;
    int data[];
};

dynarr new_dynarr(void);
void append(dynarr *a, int i);
dynarr append2(dynarr a, int i);

// (Almost) genric stack
struct stack
{
    size_t len;
    size_t cap;
    size_t frame_size;
    alignas(max_align_t) char data[];
};

typedef struct stack *stack;

stack new_stack(size_t frame_size);
bool is_empty(stack *stack);
void *top(stack *stack);
void *pop(stack *stack);
void push(stack *stack, void *frame);

// Binary trees. We don't have generics in C
// (not without a lot of hacks at least) so the
// tree is hardwired to have ints as values.
typedef struct node *tree;
struct node
{
    int value;
    tree left, right;
};

tree new_tree(int value, tree left, tree right);
void free_tree(tree t);

// Direct recursion
dynarr direct(tree t);

// CPS traversal
dynarr cps(tree t);

// Stack traversal
dynarr stack_traversal(tree t);

#endif // TRAVERSAL_H
