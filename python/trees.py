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


print(inorder(tree))

Cont = Callable[[list[T]], list[T]]


def concat(x: list[T], y: list[T]) -> list[T]:
    """Concatenate x and y."""
    x.extend(y)
    return x


def cps(t: Tree[T], k: Cont[T] = lambda x: x) -> list[T]:
    """Inorder traversal of a tree."""
    if t is None:
        return k([])

    def handle_value_and_right(left: list[T]) -> list[T]:
        def handle_right(right: list[T]) -> list[T]:
            return k(concat(left, right))
        left.append(t.value)
        return cps(t.right, handle_right)

    return cps(t.left, handle_value_and_right)


print(cps(tree))


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
        match op:
            case Op.EMIT:
                res.append(t.value)
            case Op.TRAVERSE:
                if t:
                    stack.append((Op.TRAVERSE, t.right))
                    stack.append((Op.EMIT, t))
                    stack.append((Op.TRAVERSE, t.left))
    return res


print(stack_traversal(tree))
