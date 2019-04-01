..
   Generated automatically by cpp2rst

.. highlight:: c


.. _space_partition_merge_subspaces:

merge_subspaces
===============

**Synopsis**:

.. code-block:: c

    std::pair<matrix_element_map_t, matrix_element_map_t> merge_subspaces (const
   triqs::hilbert_space::space_partition::operator_t & Cd, const
   triqs::hilbert_space::space_partition::operator_t & C, bool store_matrix_elements =
   true)

Perform Phase II of the automatic partition algorithm


Parameters
----------

 * **Cd**: Subject operator `Cd`, normally a creation operator

 * **C**: Conjugate of `Cd`, normally an annihilation operator

 * **store_matrix_elements**: Should we store the non-vanishing matrix elements of `Cd`?



Return value
------------

Non-vanishing matrix elements of `Cd` and `C`, if `store_matrix_elements = true`

Documentation
-------------


   Merge some of the invariant subspaces together, to ensure that a given operator `Cd`
   and its Hermitian conjugate `C` generate only one-to-one connections between the subspaces.