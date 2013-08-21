.. _operators:


The Operator class
===================

The TRIQS solvers need to know several operators in order to solve the impurity
problem. For example, they must know what the local Hamiltonian is, but also its
quantum numbers (that can be used to improve the speed), possibly some
operators to be averaged, aso.

In order to deal with these objects, TRIQS provides a class that allows to
manipulate operators.

A simple example
-----------------

.. literalinclude:: manipulate.py



Complete reference
------------------------

.. autoclass:: pytriqs.operators.Operator
  :members:
