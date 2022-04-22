#include "traversal.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct ttree *ttree;
typedef uintptr_t tptr; // A "tagged" pointer

struct ttree
{
    int value;
    tptr left, right;
};

// clang-format off
static inline tptr  as_tptr(ttree t)       { return (tptr)t;                      }
static inline tptr  tag(ttree t, bool tag) { return t ? (tptr)t | tag : 0;        }

static inline bool  is_tagged(tptr p)      { return (p & (tptr)1);                }
static inline ttree as_ptr(tptr p)         { return (ttree)(p & ~(tptr)1);        }
static inline ttree as_true_ptr(tptr p)    { return is_tagged(p) ? 0 : as_ptr(p); }
// clang-format on

ttree new_ttree(int value, ttree left, ttree right)
{
    ttree t = malloc(sizeof *t);
    assert(t);
    *t = (struct ttree){
        .value = value,
        .left = as_tptr(left),
        .right = as_tptr(right),
    };
    return t;
}

static inline ttree rightmost(tptr p)
{
    ttree t = as_true_ptr(p), right;
    while ((right = as_true_ptr(t->right)))
        t = right;
    return t;
}

static inline ttree leftmost(tptr p)
{
    ttree t = as_true_ptr(p), left;
    while ((left = as_true_ptr(t->left)))
        t = left;
    return t;
}

// Do a Morris traversal to connect all leave's right-child
// to the next in-order node
static void thread_tree(ttree t)
{
    ttree prev = 0;
    while (t)
    {
        if (t->left && (prev = rightmost(t->left)) && !is_tagged(prev->right))
        {
            prev->right = tag(t, true); // First time we got to prev, so thread it
            t = as_ptr(t->left);        // continue left
        }
        else
        {
            t = as_ptr(t->right);
        }
    }
}

static dynarr threaded_traversal(ttree t)
{
    dynarr a = new_dynarr();

    while (t)
    {
        t = leftmost(as_tptr(t));
        append(&a, t->value);
        if (t->right && is_tagged(t->right))
        {
            // Go to the ancestor and emit from it,
            // then we go to its right child when we
            // go right later
            t = as_ptr(t->right);
            append(&a, t->value);
        }
        t = as_ptr(t->right);
    }

    return a;
}

void free_ttree(ttree t)
{
    // FIXME: You can do this with a threaded traversal
    // instead of recursion, but I'll leave that as an exercise.
    if (t)
    {
        free_ttree(as_true_ptr(t->left));
        free_ttree(as_true_ptr(t->right));
        free(t);
    }
}

// Testing...

static dynarr threaded_traversal_wrapper(ttree t)
{
    thread_tree(t);
    return threaded_traversal(t);
}
static void test_expected(ttree t, size_t len, int expected[len])
{
    dynarr a = threaded_traversal_wrapper(t);
    assert(a->len == len);
    for (size_t i = 0; i < len; i++)
    {
        assert(expected[i] == a->data[i]);
    }
    free(a);
}

int main(void)
{
    // Add more tests as needed.

    ttree t = new_ttree(
        2,
        new_ttree(1, NULL, NULL),
        new_ttree(4,
                  new_ttree(3, NULL, NULL),
                  new_ttree(5, NULL, NULL)));
    test_expected(t, 5, (int[]){1, 2, 3, 4, 5});
    free_ttree(t);

    // 0 in expected is ignored but required by standard
    test_expected(NULL, 0, (int[]){0});

    return 0;
}
