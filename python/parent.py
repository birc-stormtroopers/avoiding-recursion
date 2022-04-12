"""Trees with parent pointers."""

from __future__ import annotations
from typing import (
    Optional, TypeVar, Generic, Iterator
)

T = TypeVar('T')


class Node(Generic[T]):
    """Inner node of a tree."""

    value: T
    left: Tree[T]
    right: Tree[T]
    parent: Tree[T]

    def __init__(self, value: T,
                 left: Tree[T] = None,
                 right: Tree[T] = None):
        """Create a new node."""
        self.value = value
        self.left = left
        if self.left:
            self.left.parent = self
        self.right = right
        if self.right:
            self.right.parent = self
        self.parent = None


# A tree is either a Node or None
Tree = Optional[Node[T]]


tree = Node(
    'A',
    Node('B', Node('D', right=Node('H')), Node('E')),
    Node('C', Node('F'), Node('G', Node('I'), Node('J')))
)

State = tuple[bool, Tree[T]]
NextState = tuple[Optional[State[T]], Optional[T]]


def step(state: State[T]) -> NextState:
    """Take one step in the traversal."""
    _, t = state  # to silence the linter that doesn't understand match
    match state:
        case(_, None):
            # If the tree is None we are done
            return None, None

        case(True, t) if t.left:
            # If we should go left and we can, go left
            return (True, t.left), None

        case(_, t) if t.right:
            # If we can go right, emit the value and go right
            # After that, we should go left again
            return (True, t.right), t.value

        case(_, t):
            # If we are in a leaf, or a node where we are not allowed
            # to go left, so we should emit the current value,
            # then search up as long as we are on a right path,
            # and stop there, in a state where we only go right
            value = t.value
            while t and t.parent and t.parent.right == t:
                t = t.parent
            return (False, t.parent), value


def inorder(t: Tree[T]) -> Iterator[T]:
    """In-order traversal (without recursion)."""
    state: State[T] = (True, t)
    while state:
        state, emission = step(state)
        if emission:
            yield emission


print(list(inorder(tree)))
