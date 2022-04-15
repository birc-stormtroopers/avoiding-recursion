#include "traversal.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

static inline size_t dynarr_size(size_t cap)
{
    struct dynarr_data dummy; // only used to compute size
    return offsetof(struct dynarr_data, data) + cap * sizeof dummy.data[0];
}

static inline dynarr realloc_dynarr(dynarr a, size_t cap)
{
    // This is usually a no-no in case of allocation errors, but
    // we ignore them and bail if it happens.
    a = realloc(a, dynarr_size(cap));
    assert(a); // Don't want to deal with allocation errors
    a->cap = cap;
    return a;
}

#define INIT_SIZE 2 // Don't make this zero (you will regret it)
dynarr new_dynarr(void)
{
    dynarr a = realloc_dynarr(0, INIT_SIZE);
    a->len = 0;
    return a;
}

void append(dynarr *a, int i)
{
    if ((*a)->len == (*a)->cap)
    {
        *a = realloc_dynarr(*a, 2 * (*a)->cap);
    }
    (*a)->data[(*a)->len++] = i;
}
