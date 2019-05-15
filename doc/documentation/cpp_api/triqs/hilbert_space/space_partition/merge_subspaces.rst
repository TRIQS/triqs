..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _space_partition_merge_subspaces:

triqs::hilbert_space::space_partition::merge_subspaces
======================================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Perform Phase II of the automatic partition algorithm`
       | std::pair<matrix_element_map_t, matrix_element_map_t> :red:`merge_subspaces` (space_partition::operator_t const & :param:`Cd`,
       |   space_partition::operator_t const & :param:`C`,
       |   bool :param:`store_matrix_elements` = true)






   Merge some of the invariant subspaces together, to ensure that a given operator `Cd`
   and its Hermitian conjugate `C` generate only one-to-one connections between the subspaces.





Parameters
^^^^^^^^^^

 * **Cd**: Subject operator `Cd`, normally a creation operator

 * **C**: Conjugate of `Cd`, normally an annihilation operator

 * **store_matrix_elements**: Should we store the non-vanishing matrix elements of `Cd`?


Returns
^^^^^^^

 * Non-vanishing matrix elements of `Cd` and `C`, if `store_matrix_elements = true`