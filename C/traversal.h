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

struct stack *new_stack(size_t frame_size);
bool is_empty(struct stack **stack);
void *pop(struct stack **stack);
void push(struct stack **stack, void *frame);

// Approximating generic data structure. The type
// check is rudamentary at best, but a little better
// and safer than the generic interface.
// clang-format off
#define STACK(T)        struct { struct stack *stack; }
#define NEW_STACK(T)    { .stack = new_stack(sizeof(T)) }
#define IS_EMPTY(S)     is_empty(&(S).stack)
#define POP(T, S)       (*(T *)pop(&(S).stack))
#define PUSH(T, S, ...) push(&(S).stack, &(T){__VA_ARGS__})
#define FREE_STACK(S)   free((S).stack)
// clang-format on

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

// Stack traversal
dynarr trampoline(tree t);

#endif // TRAVERSAL_H
