#include <stdio.h>
#include <stdlib.h>

static int fib_direct(int n)
{
    return (n < 2) ? n : (fib_direct(n - 1) + fib_direct(n - 2));
}

// Closures need to remember some of these.
struct frame
{
    int n;
    struct closure *k;
};
typedef struct frame frame;

// A continuation is an int -> int closure.
typedef int (*cont_fn)(int, frame);

// And a closure is a function + a frame.
struct closure
{
    cont_fn f;
    struct frame frame;
};
typedef struct closure closure;

// Allocating and initialising a closure
closure *new_closure(cont_fn f, int n, closure *k)
{
    closure *cl = malloc(sizeof *cl);
    cl->f = f;
    cl->frame = (frame){.n = n, .k = k};
    return cl;
}

// Call closure, but free resources first
int call_closure(closure *cl, int n)
{
    cont_fn f = cl->f;
    frame frame = cl->frame;
    free(cl);
    return f(n, frame);
}

// Fib CPS implementation
static int done(int, frame);
static int after_left(int, frame);
static int after_right(int, frame);
static int cps_rec(int, closure *);

static int done(int n, frame frame)
{
    (void)frame; // silence lint warning
    return n;
}

static int after_left(int left, frame frame)
{
    return cps_rec(frame.n - 2,
                   new_closure(after_right, left, frame.k));
}

static int after_right(int right, frame frame)
{
    return call_closure(frame.k, right + frame.n);
}

static int cps_rec(int n, closure *k)
{
    if (n < 2)
        return call_closure(k, n);
    else
        return cps_rec(n - 1, new_closure(after_left, n, k));
}

static int fib_cps(int n)
{
    return cps_rec(n, new_closure(done, 0, 0));
}

int main(void)
{
    for (int i = 0; i < 10; i++)
    {
        printf("fib(%d) == %d, fib_cps(%d) == %d\n",
               i, fib_direct(i),
               i, fib_cps(i));
    }
    return 0;
}
