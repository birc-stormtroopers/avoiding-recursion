#include "traversal.h"
#include <assert.h>
#include <stdio.h>

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

// Closures need to remember some of these.
struct frame
{
    tree t;
    struct closure *k;
};
typedef struct frame frame;

// A continuation is a dynarry -> dynarr closure.
typedef dynarr (*cont_fn)(dynarr, frame);

// And a closure is a function + a frame.
struct closure
{
    cont_fn f;
    struct frame frame;
};
typedef struct closure closure;

// Allocating and initialising a closure
closure *new_closure(cont_fn f, tree t, closure *k)
{
    closure *cl = malloc(sizeof *cl);
    cl->f = f;
    cl->frame = (frame){.t = t, .k = k};
    return cl;
}

// Call closure, but free resources first
dynarr call_closure(closure *cl, dynarr a)
{
    cont_fn f = cl->f;
    frame frame = cl->frame;
    free(cl);
    return f(a, frame);
}

// CPS traversal
dynarr done(dynarr a, frame frame);
dynarr after_left(dynarr a, frame frame);
dynarr cps_rec(tree t, dynarr a, closure *k);

dynarr done(dynarr a, frame frame)
{
    (void)frame; // Just for the linter
    return a;
}

dynarr after_left(dynarr a, frame frame)
{
    return cps_rec(frame.t->right,
                   append2(a, frame.t->value),
                   frame.k);
}

dynarr cps_rec(tree t, dynarr a, closure *k)
{
    if (t == NULL)
        return call_closure(k, a);
    else
        return cps_rec(t->left, a, new_closure(after_left, t, k));
}

dynarr cps(tree t)
{
    return cps_rec(t, new_dynarr(), new_closure(done, 0, 0));
}
