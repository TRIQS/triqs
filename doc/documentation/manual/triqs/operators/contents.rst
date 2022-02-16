.. index:: Second-quantization operators

.. module:: triqs.operators
   :noindex:

.. _operators:

**[triqs/operators]** Second-quantization operators and many-body Hamiltonians
==============================================================================

The TRIQS-based solvers need to know several operators in order to solve the impurity
problem. For example, they must know what the local Hamiltonian is, and optionally its
quantum numbers (that can be used to improve the speed). Possibly some operators of observables to be measured, also.

In order to deal with these objects, TRIQS provides a class that allows to manipulate operators.
A class with commonly-used quantities such as Hamiltonians is also provided.

.. toctree::
   :maxdepth: 2

   operators
   util/hamiltonians
   util/U_matrix
   util/observables
   util/op_struct
   util/extractors
