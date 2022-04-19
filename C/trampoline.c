#include "traversal.h"

// clang-format off
typedef STACK(struct thunk) thunk_stack;
typedef void (*thunk_fn)(dynarr *a, thunk_stack stack, tree t);
typedef struct thunk { thunk_fn fn; tree t; } thunk;

static void traverse  (dynarr *a, thunk_stack stack, tree t);
static void after_left(dynarr *a, thunk_stack stack, tree t);
// clang-format on

// Macros to make the intent of pushing clearer
#define CALL(FN, T) PUSH(thunk, stack, .fn = FN, .t = T)
#define CALL_WITH_CONT(FN, T, K, TT)          \
    do                                        \
    {                                         \
        PUSH(thunk, stack, .fn = K, .t = TT); \
        PUSH(thunk, stack, .fn = FN, .t = T); \
    } while (0);

static void traverse(dynarr *a, thunk_stack stack, tree t)
{
    if (t)
        CALL_WITH_CONT(traverse, t->left, after_left, t);
}

static void after_left(dynarr *a, thunk_stack stack, tree t)
{
    append(a, t->value);
    CALL(traverse, t->right);
}

dynarr trampoline(tree t)
{
    dynarr a = new_dynarr();
    if (t)
    {
        thunk_stack stack = NEW_STACK(thunk);
        PUSH(thunk, stack, .fn = traverse, .t = t);
        while (!IS_EMPTY(stack))
        {
            thunk th = POP(thunk, stack);
            th.fn(&a, stack, th.t);
        }
        FREE_STACK(stack);
    }
    return a;
}
