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

