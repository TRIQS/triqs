..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__hilbert_space__space_partition__constructor:

triqs::hilbert_space::space_partition::constructor
==================================================

*#include <triqs/hilbert_space/space_partition.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :red:`space_partition` (space_partition::state_t const & :param:`st`,
       |                space_partition::operator_t const & :param:`H`,
       |                bool :param:`store_matrix_elements` = true)

    2. | :red:`space_partition` (space_partition<StateType, OperatorType> const & )

Documentation



 **1)**   Perform Phase I of the automatic partition algorithm

  Partitions a Hilbert space into invariant subspaces of the Hamiltonian.



 **2)**   Copy-constructor





Parameters
^^^^^^^^^^

 * :param:`st` Sample many-body state to be used internally by the algorithm

 * :param:`H` Hamiltonian as an imperative operator

 * :param:`store_matrix_elements` Should we store the non-vanishing matrix elements of the Hamiltonian?