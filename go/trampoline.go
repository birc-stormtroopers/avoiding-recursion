package main

import "fmt"

type node[T any] struct {
	value T
	left  *node[T]
	right *node[T]
}

type thunk[T any] func() *result[T]
type result[T any] struct {
	next thunk[T]
	val  *T
}

func handleRight[T any](t *node[T], k thunk[T]) thunk[T] {
	return func() *result[T] {
		return &result[T]{next: k}
	}
}

func handleLeft[T any](t *node[T], k thunk[T]) thunk[T] {
	return func() *result[T] {
		return &result[T]{
			next: func() *result[T] { return inorder(t.right, handleRight(t, k)) },
			val:  &t.value,
		}
	}
}

func inorder[T any](t *node[T], k thunk[T]) *result[T] {
	if t == nil {
		return &result[T]{next: k}
	}

	return &result[T]{
		next: func() *result[T] { return inorder(t.left, handleLeft(t, k)) }}
}

func main() {
	tree := &node[string]{"A",
		&node[string]{"B",
			&node[string]{"D", nil, &node[string]{"H", nil, nil}},
			&node[string]{"E", nil, nil}},
		&node[string]{"C",
			&node[string]{"F", nil, nil},
			&node[string]{"G", &node[string]{"I", nil, nil}, &node[string]{"J", nil, nil}}},
	}

	res := inorder(tree, func() *result[string] { return nil })
	for ; res != nil; res = res.next() {
		if res.val != nil {
			fmt.Printf("%s ", *res.val)
		}
	}
	fmt.Println()

}
