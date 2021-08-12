.. index:: Lightweight exact diagonalization solver and tools

.. _atom_diag:

**[triqs/atom_diag]** Lightweight exact diagonalization solver and tools
========================================================================

`atom_diag` is a simple class allowing to find energy levels and eigenstates
of finite systems of fermions (atomic-like problems).

Diagonalization is preceeded by reduction of the Hamiltonian to a block-diagonal
form or, in other words, by finding its invariant subspaces in the full Hilbert
space of the problem. This step is done either using user-supplied quantum number
operators, or in an automatic fashion.

A diagonalization object can then be passed to a number of utility functions
to compute equilibrium density matrix, thermal expectation values of observables
and Matsubara Green's functions at a given temperature.

`atom_diag` comes with C++ and Python interfaces. It may prove useful as a
component to build quantum impurity solvers (hybridization expansion CT-QMC,
Hubbard-I, NCA, ...), which require solution of an auxiliary atomic problem.
However, it is no substitute for a large scale exact diagonalization solver,
since it can only treat problems of a moderate size.

.. toctree::
   :maxdepth: 1

    [atom_diag] Exact diagonalization of an atomic problem </documentation/cpp_api/triqs/atom_diag/atom_diag>
    [atom_diag_functions] Utility functions for atom_diag <functions>

Example of use: Python
----------------------

.. literalinclude:: ./atom_diag_example.py
    :language: python

Example of use: C++
--------------------

.. literalinclude:: ./atom_diag_example.cpp
    :language: cpp
