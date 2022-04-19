# C implementations

A wonderful thing about C is that it is so low-level that you get to implement most features yourself, which means that you truly get to understand them. In C we do not have closures, so we have to make our own (and thus have to understand how they work). We don't have automatic garbage collection, so where resources are used is immidiately transparent (although it is not at all a trivial task to keep track of where resources go once you have allocated them, which is why resource management is such a tricky problem that has to be handled with care).

Anyway, defining a tree structure isn't hard:

```C
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
```

There is no generics, and to keep things simple resource-wise I have defined that nodes hold an integer and two trees. A tree is a pointer to a node (`node *`) and using pointers is always necessary for recursive data structures (it's just that some languages hide this fact from you; C does not hide anything). Pointers are not automatically `nil` or `None` or anything like that, as they are in Go or Rust--in Python you can't have uninitialised variables so there is a completely different issue there--so to get empty trees we must set `left` and `right` explicitly to `NULL` pointers, writte as either `0` or `NULL`.

For creating and freeing trees I have written two functions:

```C
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
```

They are intentionally kept simple, but when we create a tree we allocate memory for it using `malloc()` and that means that we have to free it again with `free()`.

For the examples, I have written a *dynamic array*, which is essentially how Python's lists or Go's slices are implemented. You don't have to read the implementation to use it. It isn't terribly complicated if you know how to write C, but the memory management can be tricky if you don't.

The interface is this:

```C
typedef struct dynarr_data *dynarr;
struct dynarr_data
{
    size_t len;
    size_t cap;
    int data[];
};

dynarr new_dynarr(void);
void append(dynarr *a, int i);
```

A dynamic array, `dynarr` is a pointer to the structure `dynarr_data`. To get an element in a `dynarr` `a` you can use `a->data[i]`.

You create a new `dynarr` with `new_dynarr()` and you append to one with `append(&a, i)`. The `&` in front of a variable takes the address of the variable, so if `a` is a `dynarr` then `&a` is a pointer to a `dynarr` (`dynarr *`) which in turn is a pointer to a pointer to a `dynarr_data`. Why pointers to pointers, you might ask? Well, that has to do with how I have implemented the memory management of dynamic arrays. When I extend an array I might have to allocate new data, that means the existing data might move, so after appending `a` should point somewhere else. In Go, `append` returns the new address, instead, I update where `a` is pointing, but to do that, I need to address where `a` is to I can change the data there. If you append something, you need ti give `append()` the address where you have your `dynarr`.

A direct recursive in-order traversal looks like this:

```C
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
```

The `direct_rec()` function also takes a pointer to a `dynarr` as input, so I can update where the original `a` in `direct()` points. If I implemented it like this:

```C
static void direct_rec(tree t, dynarr a)
{
    if (t)
    {
        direct_rec(t->left, a);
        append(&a, t->value);
        direct_rec(t->right, a);
    }
}
```

and gave `append()` the address of the argument `a`, I could still append but it would be the *local* variable `a` that gets updated if I have to move the data in memory. If I then tried to access it using the original pointer, Bad Things would happen. (The C standard calls Bad Things "Undefined behaviour", but in my experience that never means Good Things).

That's the simple basic stuff. Now let's do something interesting.

## Closures in C

Since C doesn't have closures, we obviously cannot use them to implement a CPS solution to anything. But we can implement our own, and that way learn what a closure really is.

Implementing general closures that can do anything a closure in another language can do is highly complex. Dealing with the types that closures can have--what arguments they take, what they return, how long they live before we have to reclaim the resources the use is crazy complicated. But it isn't too hard to implement concrete kinds of closures for the specific application we have in mind.

To make things simpler, we can start with computing the Fibonacci numbers. A version with direct recursion could look like this:

```C
int fib_direct(int n)
{
    return (n < 2) ? n : (fib_direct(n - 1) + fib_direct(n - 2));
}
```

If we rewrite it a little, the return points from the recursions are clearer:

```C
int fib_direct(int n)
{
    if (n < 2)
    {
        return n;
    }
    else
    {
        int left = fib_direct(n - 1);
        int right = fib_direct(n - 2);
        return left + right;
    }
}
```

A CPS version would need to handle two continuations: when we return from the left recursion

```C
=>      int right = fib_direct(n - 2);
        return left + right;
```

where we need to call the right recursion, and another when we return from the right recursion and need to add the two results

```C
=>      return left + right;
```

The continuations get the result from a recursive call, so an integer, and have to remember some context--the closure--which we have to implement explicitly. What the closures have to remember is an integer (`n` when we return from the left recursion, so we can recurse on the right, and the `left` result when we return from the right recursion so we can add the results) and they need to remember a continuation to call when they are done with their bits.

So, we can implement a continuation/closure like this:

```C
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
```

A frame, what the closure remembers, is an integer and a closure, the functions in a closure takes an integer and a frame as argument and returns an integer, and a closure wraps a function together with a frame.

To create and call closures, we can add some convinience functions:

```C
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
```

I `malloc()` memory for closures, because I need the memory and can't put it on the stack. That means I also have to `free()` closures later. I've implemented it such that a closure is deleted when you call it. First we extract the data in it, so that doesn't get deleted, then we free the closure, and finally we call the function with the frame.

Using `malloc()` and `free()` is not an efficient solution, but it is simple. Besides the inefficiency, it also holds the danger of leaking memory. If we do not call *all* the closures we allocate, some will not be deallocated, and that will leave resources unclaimed. That isn't a problem in this application, but it could be if we used these closures in something like a search where we would return early when we found what we were looking for.

A better solution would be to have a custom allocator, and because we create and call closures in a stack order, it isn't hard to write, but we will leave that for another day.

For now, let's focus on how we use the closures.

C's scope rules go "top-down" in the file, so if we want to call functions recursively we have to tell C that they exist with prototypes, so we do that first:

```C
// Fib CPS implementation
static int done(int, frame);
static int after_left(int, frame);
static int after_right(int, frame);
static int cps_rec(int, closure *);
```

The `done()` function is the one we will call when the recursion is done, the `after_left()` and `after_right()` functions should be self-explanatory, and the `cps_rec()` is not a continuation but the recursive function. It takes a continuation closure as an argument, while the closures take a `frame`.

The actual implementation is straightforward:

```C
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
```

If you compile this code with optimisation, you should get tail-call optimisation. At least [that is what I see.](https://godbolt.org/z/eq5EqMhY7). If you look at the generated assembly, ther are `jmp` instructions at the end of the functions instead of `ret`, in the cases where you have a tail-call. That means that we don't need to do anything else to avoid explicit recursion and for reducing the stack usage.

It doesn't mean that this is as efficient as normal function calls. Calling a function through a pointer messes with the CPU's branch-prediction and that slows the execution down, by a lot, compared to jumping to a know location. So, using an explicit stack to keep track of the state of a recursion (or in the case of `fib()` not using much state at all since you only need two integers) will be faster. It can be more unmanagable, though, and then it is good to know that CPS can do the trick. Check the generated code, however! I have been bitten by cases where the compiler *didn't* do tail-call optimisation where I expected it to, and running out of stack space in C is an unpleasant experience.

Getting back to the tree traversal, we can implement that in a form that very closely resembles the Fibonacci calculation. We need closures that remember a tree and a continuation, instead of an integer and a continuation, and they will become functions from dynamic arrays to dynamic arrays, but otherwise nothing changes:

```C
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
```

The closures didn't have to return dynamic arrays, we could keep references to one and save it in closures or get them as arguments, but now I'm doing it this way.

The traversal is, if anything, simpler than the `fib()` solution:

```C
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
```

I'm using a new append function, `append2()`, that returns the updated dynamic array. It is slightly easier to use in this solution than the one that takes a pointer to a `dynarr` and doesn't return anything. You could use the old one instead, of course.

If you are unlucky and the compiler doesn't optimise tail-calls, you can translate this version into one that uses a trampoline in much the same way as in Python. But since we are in C, we will try making an efficient solution as well. Before that, though, let's quickly consider the stack solution.

## Explicit stack

There is nothing special about the stack solution in C, except for how you implement a stack, and I suggest you don't look too closely at my implementation today.[^1] The solution looks like this:

```C
enum op { TRAVERSE, EMIT };
struct frame { enum op op; tree t; };

dynarr stack_traversal(tree t)
{
    dynarr a = new_dynarr();
    STACK(struct frame) stack = NEW_STACK(struct frame);

    PUSH(struct frame, stack, .op = TRAVERSE, .t = t);
    while (!IS_EMPTY(stack))
    {
        struct frame frame = POP(struct frame, stack);
        if (!frame.t)
            continue;
        switch (frame.op)
        {
        case TRAVERSE:
            PUSH(struct frame, stack, .op = TRAVERSE, .t = frame.t->right);
            PUSH(struct frame, stack, .op = EMIT, .t = frame.t);
            PUSH(struct frame, stack, .op = TRAVERSE, .t = frame.t->left);
            break;
        case EMIT:
            append(&a, frame.t->value);
            break;
        }
    }
    FREE_STACK(stack);

    return a;
}
```

There is nothing to it, but I want to write a trampoline version of the traversal based on this stack. The closures in the CPS are created and called in a stack-like fashion, which means that I can get a more efficient memory management, and one where freeing resources if I terminate earlier is a lot simpler.[^2] 

I will use a stack where I push closures, and where I pop and call closures until the stack is empty. It does exactly what the code above does, and about as efficient as well, it just does so without a big hunking `switch`-statement to drive the control flow.

## Thunks jumping up and down on the stack

If you always use closures in a stack-like fashion, then instead of calling one, you can just push it on the stack, but if you want to call a function *after* another call, the way we do with continuation, you just push the continuation onto the stack before the main function. This is how continuations work when you simulate recursion with them, and why continuations are generalisations of normal function semantics (you can use them exactly the way you use recursive functions, but you can also do more with them, as they don't always have to be called in stack order).

Here's the CPS version of the tree traversal when we use a stack:

```C
typedef STACK(struct thunk) thunk_stack;
typedef void (*thunk_fn)(dynarr *a, thunk_stack stack, tree t);
typedef struct thunk { thunk_fn fn; tree t; } thunk;

static void traverse  (dynarr *a, thunk_stack stack, tree t);
static void after_left(dynarr *a, thunk_stack stack, tree t);

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
    if (t) CALL_WITH_CONT(traverse, t->left, after_left, t);
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
```

I hope you agree that it isn't frightfully more complicated than the explicit stack version. It is a little more complicated since we have had to move the two `case`-statements from the `switch` into separate functions, and therein lies both the pro and con of using this technique.

If you have something as simple as an in-order tree traversal, then an explicit stack is easy to manage, and it just gets more complicated if you split some of the code's logic into separate functions. However, in more complicated applications, where you may have interleaved computations that call each other--an approximate pattern matching where one recursion explores different patterns and another explores an index structure comes to mind for those who've taken GSA--a bit `switch`-statement also becomes unmanageable. There, it is sometimes easier to keep the different logically connected parts of the traversal together and not mixed in a `switch`, and if you do that with thunks you can still interleave them the way we just saw. 



[^1]: C doesn't implement generics, but I have implemented a generic stack anyway, and naturally that can get a little tricky. To do this, you have to work with raw memory at a low level, and that might be a topic for another day, but it won't be today.

[^2]: All the CPS examples we have seen create and call closures in the same stack-like way that direct recursion would, so naturally they can use a stack to store their data. Closures don't have to, of course--and you have all used closures in higher-order functions that are not used that way. This enables a very flexible control flow, where you can encode everything from normal function calls over exception-throwing semantics to general co-routines. But maybe that is better left for another day as well.