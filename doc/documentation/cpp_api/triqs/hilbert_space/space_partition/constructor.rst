..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _space_partition_constructor:

triqs::hilbert_space::space_partition::constructor
==================================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Perform Phase I of the automatic partition algorithm`
       | :red:`space_partition` (space_partition::state_t const & :param:`st`,
       |   space_partition::operator_t const & :param:`H`,
       |   bool :param:`store_matrix_elements` = true)

    2. | :cppbrief:`Copy-constructor`
       | :red:`space_partition` (space_partition<StateType, OperatorType> const & )

Documentation



 **1)**
   Partitions a Hilbert space into invariant subspaces of the Hamiltonian.





Parameters
^^^^^^^^^^

 * **st**: Sample many-body state to be used internally by the algorithm

 * **H**: Hamiltonian as an imperative operator

 * **store_matrix_elements**: Should we store the non-vanishing matrix elements of the Hamiltonian?