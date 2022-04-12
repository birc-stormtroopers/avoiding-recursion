package main

import "fmt"

type node[T any] struct {
	value T
	left  *node[T]
	right *node[T]
}

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

func main() {
	tree := &node[string]{"A",
		&node[string]{"B",
			&node[string]{"D", nil, &node[string]{"H", nil, nil}},
			&node[string]{"E", nil, nil}},
		&node[string]{"C",
			&node[string]{"F", nil, nil},
			&node[string]{"G", &node[string]{"I", nil, nil}, &node[string]{"J", nil, nil}}},
	}
	fmt.Println(inorder(tree, func(x []string) []string { return x }))
}
