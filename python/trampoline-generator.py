"""Working with trampolines."""

from typing import Callable, Union, Any, Optional, Iterator
from trees import (
    Node, Tree, tree, T
)

# Shouldn't be Any but type checker can't handle cycles
Thunk = Callable[[], Any]
Res = Union[None, tuple[Optional[T], Thunk]]
Cont = Callable[[], Res[T]]


def handle_right(k: Cont[T]) -> Cont[T]:
    """Make the 'go right' continuation."""
    def cont() -> Res[T]:
        """Continuation when we return from right."""
        return (None, k)
    return cont


def handle_left(t: Node[T], k: Cont[T]) -> Cont[T]:
    """Make the 'go left' continuation."""
    def cont() -> Res[T]:
        """Continuation when we return from left."""
        return (t.value, lambda: cps(t.right, handle_right(k)))
    return cont


def cps(t: Tree[T], k: Cont[T] = lambda: None) -> Res[T]:
    """Inorder traversal of a tree."""
    return k() if not t else (None, lambda: cps(t.left, handle_left(t, k)))


def in_order(t: Tree[T]) -> Iterator[T]:
    """Compute the in-order traversal of t."""
    res = cps(t)
    while res:
        val, thunk = res
        if val:
            yield val
        res = thunk()


print(list(in_order(tree)))
