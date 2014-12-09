.. module:: pytriqs.random 

.. _operators:

Many-body operators
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
    H = c('up',1) * c_dag('up',2) + c('up',2) * c_dag('up',1)
    print H
    print H - H.dagger()
    print anti_commutator(c('up'),c_dag('up'))
    print anti_commutator(c('up'),0.5*c_dag('down'))


Full reference
------------------------

.. autoclass:: pytriqs.operators.Operator
  :members:
