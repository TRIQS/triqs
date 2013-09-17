.. module:: pytriqs.random 

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

.. runblock:: python

    from pytriqs.operators import *
    H = C('up',1) * Cdag('up',2) + C('up',2) * Cdag('up',1)
    print H
    print H - H.dagger()
    print anti_commutator(C('up'),Cdag('up'))
    print anti_commutator(C('up'),0.5*Cdag('down'))


Complete reference
------------------------

.. autoclass:: pytriqs.operators.Operator
  :members:
