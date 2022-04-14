package main

import "fmt"

type node[T any] struct {
	value T
	left  *node[T]
	right *node[T]
}

func cps[T any](t *node[T], acc []T, k func([]T) []T) []T {
	if t == nil {
		return k(acc)
	}

	handleValueAndRight := func(acc []T) []T {
		handleRight := func(acc []T) []T {
			acc = append(acc, t.value)
			return k(acc)
		}
		return cps(t.right, acc, handleRight)
	}

	return cps(t.left, acc, handleValueAndRight)
}

func inorder[T any](t *node[T]) []T {
	return cps(t, []T{}, func(x []T) []T { return x })
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
	fmt.Println(inorder(tree))
}
