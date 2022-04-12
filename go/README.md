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

