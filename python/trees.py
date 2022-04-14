"""Traversing trees."""

from __future__ import annotations
from typing import (
    Optional, TypeVar, Generic,
    Callable
)
from dataclasses import dataclass
from enum import Enum

T = TypeVar('T')


@dataclass
class Node(Generic[T]):
    """Inner node of a tree."""

    value: T
    left: Tree[T] = None
    right: Tree[T] = None


# A tree is either a Node or None
Tree = Optional[Node[T]]


tree = Node(
    'A',
    Node('B', Node('D', right=Node('H')), Node('E')),
    Node('C', Node('F'), Node('G', Node('I'), Node('J')))
)


def inorder(t: Tree[T], acc: list[T] | None = None) -> list[T]:
    """Inorder traversal of a tree."""
    acc = acc if acc is not None else []
    if t:
        inorder(t.left, acc)
        acc.append(t.value)
        inorder(t.right, acc)
    return acc


def cps_rec(t: Tree[T], acc: list[T], k: Callable[[], None]) -> None:
    """CPS inorder traversal of a tree."""
    if t is None:
        return k()

    def handle_value() -> None:
        assert t is not None  # For the type-checker
        acc.append(t.value)
        return cps_rec(t.right, acc, k)

    return cps_rec(t.left, acc, handle_value)


def cps(t: Tree[T]) -> list[T]:
    """Inorder traversal of a tree."""
    res: list[T] = []
    cps_rec(t, res, lambda: None)
    return res


class Op(Enum):
    """
    Operations we need to do with objects on the stack.

    These roughly correspond to the continuation points in
    the recursive solution.
    """

    TRAVERSE = 1
    EMIT = 2


def stack_traversal(t: Tree[T]) -> list[T]:
    """Inorder traversal of a tree."""
    stack = [(Op.TRAVERSE, t)]
    res = []
    while stack:
        op, t = stack.pop()
        if op == Op.EMIT:
            assert t is not None
            res.append(t.value)
        else:
            if t:
                stack.append((Op.TRAVERSE, t.right))
                stack.append((Op.EMIT, t))
                stack.append((Op.TRAVERSE, t.left))
    return res
