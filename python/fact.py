"""Different ways of computing factorial."""

from typing import Callable


def direct(n: int) -> int:
    """
    Compute the factorial of n.

    >>> direct(1)
    1
    >>> direct(2)
    2
    >>> direct(4)
    24
    """
    return n if n == 1 else n * direct(n - 1)


def tail_recursion(n: int, acc: int = 1) -> int:
    """
    Compute the factorial of n.

    >>> tail_recursion(1)
    1
    >>> tail_recursion(2)
    2
    >>> tail_recursion(4)
    24
    """
    return acc if n == 1 else tail_recursion(n - 1, n * acc)


def cps(n: int, k: Callable[[int], int] = lambda n: n) -> int:
    """
    Compute the factorial of n.

    >>> cps(1)
    1
    >>> cps(2)
    2
    >>> cps(4)
    24
    """
    return k(1) if n == 1 else cps(n - 1, lambda res: k(n * res))


def loop(n: int) -> int:
    """
    Compute the factorial of n.

    >>> loop(1)
    1
    >>> loop(2)
    2
    >>> loop(4)
    24
    """
    acc = 1
    for i in range(1, n + 1):
        acc *= i
    return acc
