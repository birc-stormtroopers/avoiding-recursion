package main

import "fmt"

type node struct {
	value string
	left  *node
	right *node
}

func inorder(t *node, k func([]string) []string) []string {
	if t == nil {
		return k([]string{})
	}

	handleValueAndRight := func(left []string) []string {
		handleRight := func(right []string) []string {
			left = append(left, t.value)
			return k(append(left, right...))
		}
		return inorder(t.right, handleRight)
	}

	return inorder(t.left, handleValueAndRight)
}

func main() {
	tree := &node{"A",
		&node{"B",
			&node{"D", nil, &node{"H", nil, nil}},
			&node{"E", nil, nil}},
		&node{"C",
			&node{"F", nil, nil},
			&node{"G", &node{"I", nil, nil}, &node{"J", nil, nil}}},
	}
	fmt.Println(inorder(tree, func(x []string) []string { return x }))
}
