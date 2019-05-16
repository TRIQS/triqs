..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__atom_diag__atom_diag__constructor:

triqs::atom_diag::atom_diag::constructor
========================================

*#include <triqs/atom_diag/atom_diag.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :red:`atom_diag` ()

    2. | :red:`atom_diag` (atom_diag::many_body_op_t const & :param:`h`,
       |          :ref:`fundamental_operator_set <triqs__hilbert_space__fundamental_operator_set>` const & :param:`fops`)

    3. | :red:`atom_diag` (atom_diag::many_body_op_t const & :param:`h`,
       |          :ref:`fundamental_operator_set <triqs__hilbert_space__fundamental_operator_set>` const & :param:`fops`,
       |          int :param:`n_min`,
       |          int :param:`n_max`)

    4. | :red:`atom_diag` (atom_diag::many_body_op_t const & :param:`h`,
       |          :ref:`fundamental_operator_set <triqs__hilbert_space__fundamental_operator_set>` const & :param:`fops`,
       |          std::vector<many_body_op_t> const & :param:`qn_vector`)

Documentation



 **1)**   Construct in an uninitialized state.



 **2)**   Reduce a given Hamiltonian to a block-diagonal form and diagonalize it
This constructor calls the auto-partition procedure, and the QR algorithm
to diagonalize the blocks. The invariant subspaces of the Hamiltonian are
chosen such that all creation and annihilation operators from the provided
fundamental operator set map one subspace to one subspace.



 **4)**   Reduce a given Hamiltonian to a block-diagonal form and diagonalize it
This constructor uses quantum number operators to partition the Hilbert space into
invariant subspaces, and the QR algorithm to diagonalize the blocks of the Hamiltonian.
The quantum numbers must be chosen such that all creation and annihilation operators from
the provided fundamental operator set map one subspace to one subspace.





Parameters
^^^^^^^^^^

 * :param:`h` Hamiltonian operator to be diagonalized.

 * :param:`fops` Fundamental operator set; Must at least contain all fundamental operators met in `h`.

 * :param:`qn_vector` Vector of quantum number operators.