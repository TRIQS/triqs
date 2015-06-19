
Hamiltonians
============

.. highlight:: python

A set of python functions to construct of standard Hamiltonians is provided in the module ``pytriqs.operators.util.hamiltonians``.
Namely, three standard interacting local Hamiltonians are implemented: Slater, (Hubbard-)Kanamori and density-density Hamiltonians.
The Hamiltonians can be written for any set of spin and orbital names. Note that there is no actual notion of spin, the spin names
simply identify a division of the GF structure into non-hybridizing blocks.

.. automodule:: pytriqs.operators.util.hamiltonians
   :members:

U matrix construction
=====================

Tools to construct the U matrices above are provided in the ``pytriqs.operators.util.U_matrix`` module.

.. automodule:: pytriqs.operators.util.U_matrix
   :members:

Helper functions
================

.. automodule:: pytriqs.operators.util.op_struct
   :members:
