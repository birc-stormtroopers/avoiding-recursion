#include "traversal.h"
#include <assert.h>

tree new_tree(int value, tree left, tree right)
{
    tree t = malloc(sizeof *t);
    assert(t);
    *t = (struct node){.value = value, .left = left, .right = right};
    return t;
}

// Using recursion here because I'm lazy, but you really
// should use a traversal that doesn't, or better still
// a custom allocator.
void free_tree(tree t)
{
    if (t)
    {
        free_tree(t->left);
        free_tree(t->right);
        free(t);
    }
}

// Direct recursive traversal
static void direct_rec(tree t, dynarr *a)
{
    if (t)
    {
        direct_rec(t->left, a);
        append(a, t->value);
        direct_rec(t->right, a);
    }
}
dynarr direct(tree t)
{
    dynarr a = new_dynarr();
    direct_rec(t, &a);
    return a;
}
