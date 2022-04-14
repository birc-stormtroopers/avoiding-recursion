"""Working with trampolines."""

from typing import Callable, Union, Any, cast
from trees import (
    Node, Tree, tree, T
)

# Shouldn't be Any but type checker can't handle cycles
Thunk = Callable[[], Union[None, Any]]
Cont = Callable[[], Union[None, Thunk]]


def after_left(t: Node[T], acc: list[T], k: Cont) -> Cont:
    """Make the 'go left' continuation."""
    def cont() -> Thunk:
        """Continuation when we return from left."""
        acc.append(t.value)
        return lambda: cps(t.right, acc, k)
    return cont


def cps(t: Tree[T], acc: list[T], k: Cont = lambda: None) -> Thunk:
    """Inorder traversal of a tree."""
    if t is None:
        return k
    return lambda: cps(t.left, acc, after_left(t, acc, k))


def in_order(t: Tree[T]) -> list[T]:
    """Compute the in-order traversal of t."""
    res: list[T] = []
    thunk = cps(t, res)
    while callable(thunk):
        thunk = thunk()
    return res


print(in_order(tree))
