#ifndef TRAVERSAL_H
#define TRAVERSAL_H

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

#endif // TRAVERSAL_H
