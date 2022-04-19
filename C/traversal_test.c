#include "traversal.h"
#include <assert.h>
#include <stdio.h>

typedef dynarr (*traversal)(tree);
static void test_expected(traversal trav, tree t,
                          size_t len, int expected[len])
{
    dynarr a = trav(t);
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

    tree t = new_tree(
        2,
        new_tree(1, NULL, NULL),
        new_tree(4,
                 new_tree(3, NULL, NULL),
                 new_tree(5, NULL, NULL)));
    test_expected(direct, t, 5, (int[]){1, 2, 3, 4, 5});
    test_expected(cps, t, 5, (int[]){1, 2, 3, 4, 5});
    test_expected(stack_traversal, t, 5, (int[]){1, 2, 3, 4, 5});
    free_tree(t);

    // 0 in expected is ignored but required by standard
    test_expected(direct, NULL, 0, (int[]){0});
    test_expected(cps, NULL, 0, (int[]){0});
    test_expected(stack_traversal, NULL, 0, (int[]){0});

    return 0;
}
