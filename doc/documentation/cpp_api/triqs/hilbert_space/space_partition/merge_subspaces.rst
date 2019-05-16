..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__hilbert_space__space_partition__merge_subspaces:

triqs::hilbert_space::space_partition::merge_subspaces
======================================================

*#include <triqs/hilbert_space/space_partition.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | std::pair<matrix_element_map_t, matrix_element_map_t> :red:`merge_subspaces` (space_partition::operator_t const & :param:`Cd`,
       |                  space_partition::operator_t const & :param:`C`,
       |                  bool :param:`store_matrix_elements` = true)





Perform Phase II of the automatic partition algorithm

  Merge some of the invariant subspaces together, to ensure that a given operator `Cd`
  and its Hermitian conjugate `C` generate only one-to-one connections between the subspaces.





Parameters
^^^^^^^^^^

 * :param:`Cd` Subject operator `Cd`, normally a creation operator

 * :param:`C` Conjugate of `Cd`, normally an annihilation operator

 * :param:`store_matrix_elements` Should we store the non-vanishing matrix elements of `Cd`?


Returns
^^^^^^^

Non-vanishing matrix elements of `Cd` and `C`, if `store_matrix_elements = true`