# Go implementation of CPS in-order traversal

In this directory I have included Go implementations of two of the basic ideas in using CPS to avoid recursion. They follow the Python code closely, so they should be easy to read, and it might motivate you to try out Go for other projects. It is a nice and simple language and *much* faster than Python. I'm using genrics in the code, so you need at least version 1.18 to compile it.

In `trees.go` you will find a `node` structure similar to the nodes in the Python code.

```go
type node[T any] struct {
	value T
	left  *node[T]
	right *node[T]
}
```

The `[T any]` says that `T` stands for any type and the `*node[T]` bits say that `left` and `right` are pointers to such nodes. Pointers can be `nil`, which serves the same purpose as `None` here.

The CPS traversal looks much like the first version we had in Python:

```go
func inorder[T any](t *node[T], k func([]T) []T) []T {
	if t == nil {
		return k([]T{})
	}

	handleValueAndRight := func(left []T) []T {
		handleRight := func(right []T) []T {
			left = append(left, t.value)
			return k(append(left, right...))
		}
		return inorder(t.right, handleRight)
	}

	return inorder(t.left, handleValueAndRight)
}
```

The `[]T{}` is an empty "list" (a *slice* is what it is in Go) and `append(left, t.value)` works like `left.append(t.value)` in Python, and `append(left, right...)` works like `left.extend(right)`. A main difference is that `append` returns the new slice, which may or may not be the same as the old one--if it has to allocate more memory to fit new data into it, it returns a new object.

It might not be the fastest way to implement an in-order traversal in Go--an explicit stack would certainly be faster--but it illustrates how CPS can be used, and it is a cool tool to have if things get more complicated than this.

I would have expected Go to do tail-call optimisation here, but at the time of writing, it [looks to me like the assembly uses function calls instead of jumps](https://godbolt.org/z/YjeajnEGh), so we would still use lots of stack space for large trees in this language.

If we are implementing a trampoline anyway, we might as well turn the traversal into an iterator (of a sort) at the same time, and get the visited nodes one at a time while we traverse.

We don't have union types in Go, but we need a way to specify that we have a new value, and a way to specify that the iteration is over, and we can use pointers (`*`) for both. A value of `nil` then means that we don't have a value, or that the iteration is over. We define a thunk as something that returns a pointer to a result (with `nil` to indicate that we are done) and a result consists of a thunk to compute the next value and a pointer to a value (with `nil` indicating that we don't have a value yet).

```go
type thunk[T any] func() *result[T]
type result[T any] struct {
	next thunk[T]
	val  *T
}
```

If we move the inner functions out of `inorder()` things look a little simpler. The function we call when are done with the right tree shoudl just give us the original continuation back:

```go
func handleRight[T any](t *node[T], k thunk[T]) thunk[T] {
	return func() *result[T] {
		return &result[T]{next: k}
	}
}
```

The function that is called when we have traversed the left tree should give us the node's value together with a thunk that traverses the right tree:

```go
func handleLeft[T any](t *node[T], k thunk[T]) thunk[T] {
	return func() *result[T] {
		return &result[T]{
			next: func() *result[T] { return inorder(t.right, handleRight(t, k)) },
			val:  &t.value,
		}
	}
}
```

and the `inorder()` function simplifies to handling leaves, by defering to the continuation, and recursing left otherwise.

```go
func inorder[T any](t *node[T], k thunk[T]) *result[T] {
	if t == nil {
		return &result[T]{next: k}
	}

	return &result[T]{
		next: func() *result[T] { return inorder(t.left, handleLeft(t, k)) }}
}
```

You can use it to traverse a tree like this:

```go
	res := inorder(tree, func() *result[string] { return nil })
	for ; res != nil; res = res.next() {
		if res.val != nil {
			fmt.Printf("%s ", *res.val)
		}
	}
	fmt.Println()
```

see `trampoline.go` for the full implementation.

If you want to run the code in one of the two files you can use

```sh
> go run trees.go
```

or

```sh
> go run trampoline.go
```

or you can compile the code first and then run it with

```sh
> go build trees.go
> ./trees
```

If you want to learn how to compile Go code in general, that will be a topic for another day.
