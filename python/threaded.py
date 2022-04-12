"""Threaded trees."""


from __future__ import annotations
from typing import (
    Optional, TypeVar, Generic,
    Iterator
)
from dataclasses import dataclass

T = TypeVar('T')


@dataclass
class Node(Generic[T]):
    """Inner node of a (left-)threaded tree."""

    value: T
    left: Tree[T] = None
    right: Tree[T] = None
    thread: Tree[T] = None  # points to the next node in order


# A tree is either a Node or None
Tree = Optional[Node[T]]


tree = Node(
    'A',
    Node('B', Node('D', right=Node('H')), Node('E')),
    Node('C', Node('F'), Node('G', Node('I'), Node('J')))
)


def rightmost(t: Node[T], sentinel: Tree[T]) -> Node[T]:
    """Find the right-most node under t."""
    while t.right and t.right != sentinel:
        t = t.right
    return t


def morris_traversal(t: Tree[T]) -> Iterator[T]:
    """In-order traversal of t."""
    while t:
        if not t.left:
            # We can't go left, so emit and go right
            yield t.value
            t = t.right
        else:
            # We can go left, so fetch the rightmost to prepare
            right = rightmost(t.left, t)
            if right.right == t:
                # If right.right points here, we must have
                # returned here through a traversal, so
                # yield the value and go right
                yield t.value
                right.right = None
                t = t.right
            else:
                # We are not in a loop, so remember
                # the thread and go left.
                right.right = t
                t = t.left


print(list(morris_traversal(tree)))


def thread(prev: Tree[T], next_node: Node[T]) -> None:
    """Set prev's thread pointer if prev isn't None."""
    if prev:
        prev.thread = next_node


def morris_thread(t: Tree[T]) -> None:
    """In-order traversal of t."""
    prev = None
    while t:
        if not t.left:
            thread(prev, t)
            prev, t = t, t.right
        else:
            right = rightmost(t.left, t)
            if right.right == t:
                thread(prev, t)
                right.right = None
                prev, t = t, t.right
            else:
                right.right = t
                t = t.left


morris_thread(tree)


def leftmost(t: Tree[T]) -> Tree[T]:
    """Find the left-most node in t."""
    if t is None:
        return None
    while t.left:
        t = t.left
    return t


def thread_traverse(t: Tree[T]) -> Iterator[T]:
    """In-order traversal following threads."""
    t = leftmost(t)
    while t:
        yield t.value
        t = t.thread


print(list(thread_traverse(tree)))
