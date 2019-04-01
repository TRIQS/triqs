..
   Generated automatically by cpp2rst

.. highlight:: c


.. _atom_diag_constructor:

constructor
===========

**Synopsis**:

.. code-block:: c

    void atom_diag<C> ()                                                                                                                                                                              (1)

    void atom_diag<C> (const triqs::atom_diag::atom_diag::many_body_op_t & h, const
   triqs::hilbert_space::fundamental_operator_set & fops)                                                        (2)

    void atom_diag<C> (const triqs::atom_diag::atom_diag::many_body_op_t & h, const
   triqs::hilbert_space::fundamental_operator_set & fops, int n_min, int n_max)                                  (3)

    void atom_diag<C> (const triqs::atom_diag::atom_diag::many_body_op_t & h, const
   triqs::hilbert_space::fundamental_operator_set & fops, const
   std::vector<many_body_op_t> & qn_vector)     (4)

(1)Construct in an uninitialized state.


(2)Reduce a given Hamiltonian to a block-diagonal form and diagonalize it

.. note::
    See :ref:`space_partition` for more details on the auto-partition scheme.

Parameters
----------

 * **h**: Hamiltonian operator to be diagonalized.

 * **fops**: Fundamental operator set; Must at least contain all fundamental operators met in `h`.



Documentation
-------------


 This constructor calls the auto-partition procedure, and the QR algorithm
 to diagonalize the blocks. The invariant subspaces of the Hamiltonian are
 chosen such that all creation and annihilation operators from the provided
 fundamental operator set map one subspace to one subspace.



(3)


(4)Reduce a given Hamiltonian to a block-diagonal form and diagonalize it


Parameters
----------

 * **h**: Hamiltonian operator to be diagonalized.

 * **fops**: Fundamental operator set; Must at least contain all fundamental operators met in `h`.

 * **qn_vector**: Vector of quantum number operators.


Documentation
-------------


 This constructor uses quantum number operators to partition the Hilbert space into
 invariant subspaces, and the QR algorithm to diagonalize the blocks of the Hamiltonian.
 The quantum numbers must be chosen such that all creation and annihilation operators from
 the provided fundamental operator set map one subspace to one subspace.