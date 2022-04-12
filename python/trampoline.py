"""Working with trampolines."""

from typing import Callable, Union, Any
from trees import (
    Node, Tree, tree, T
)

# Shouldn't be Any but type checker can't handle cycles
Thunk = Callable[[], Any]
Cont = Callable[[T], Union[T, Thunk]]


def handle_right(left: list[T], k: Cont[list[T]]) -> Cont[list[T]]:
    """Make the 'go right' continuation."""
    def cont(right: list[T]) -> Thunk:
        """Continuation when we return from right."""
        left.extend(right)
        return lambda: k(left)
    return cont


def handle_left(t: Node[T], k: Cont[list[T]]) -> Cont[list[T]]:
    """Make the 'go left' continuation."""
    def cont(left: list[T]) -> Thunk:
        """Continuation when we return from left."""
        left.append(t.value)
        return lambda: cps(t.right, handle_right(left, k))
    return cont


def cps(t: Tree[T], k: Cont[list[T]] = lambda x: x) -> Thunk:
    """Inorder traversal of a tree."""
    return lambda: (k([]) if t is None else cps(t.left, handle_left(t, k)))


def in_order(t: Tree[T]) -> list[T]:
    """Compute the in-order traversal of t."""
    thunk = cps(t)
    while callable(thunk):
        thunk = thunk()
    return thunk


print(in_order(tree))
