# Python code

This directory contains the Python code described in the main README. You can find the explanations there.

* `fact.py` -- Implementations of factorial, illustrating different ways of implementing recursion.
* `trees.py` -- In-order traversal of a binary tree using CPS and an explicit stack.
* `trampoline.py` -- The CPS solution transformed to use a trampoline, thus getting around the lack of tail-call optimisation.
* `trampoline-generator.py` -- A trampoline solution that emits the node-values during the traversal.
* `threaded.py` -- Morris-traversal and threading the tree for easier subsequent traversal.
* `parent.py` -- Adding `parent` pointers to the tree and traversing the tree using those.
