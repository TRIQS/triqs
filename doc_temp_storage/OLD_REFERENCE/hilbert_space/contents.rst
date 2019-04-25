.. index:: Hilbert space tools

.. _hilbert_space_contents:

**[triqs/hilbert_space]** Many-body states and Hilbert spaces
=============================================================

`hilbert_space` toolbox provides the user with a small set of abstractions, useful
for doing calculations in the Hamiltonian picture for finite fermionic systems.
Among these abstarctions are Hilbert spaces, their subspaces, states and operators acting on them.
There is also an efficient implementation of the automatic partitioning algorithm
that allows to split a Hilbert space into a number of subspaces, each invariant
under action of a given operator.

.. toctree::
   :maxdepth: 2

   [fundamental_operator_set] Fundamental operator set </cpp2rst_generated/triqs/hilbert_space/fundamental_operator_set>
   [hilbert_space] Full Hilbert space </cpp2rst_generated/triqs/hilbert_space/hilbert_space>
   [sub_hilbert_space] Hilbert subspace </cpp2rst_generated/triqs/hilbert_space/sub_hilbert_space>
   [state] Many-body state </cpp2rst_generated/triqs/hilbert_space/state>
   [imperative_operator] Imperative operator </cpp2rst_generated/triqs/hilbert_space/imperative_operator>
   [autopartition] Automatic partitioning algorithm </cpp2rst_generated/triqs/hilbert_space/space_partition>

Example of use
--------------

.. triqs_example:: ./hilbert_example.cpp
