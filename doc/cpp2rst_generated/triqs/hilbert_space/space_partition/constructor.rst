..
   Generated automatically by cpp2rst

.. highlight:: c


.. _space_partition_constructor:

constructor
===========

**Synopsis**:

.. code-block:: c

    void space_partition<StateType, OperatorType> (const
   triqs::hilbert_space::space_partition::state_t & st, const
   triqs::hilbert_space::space_partition::operator_t & H, bool store_matrix_elements =
   true)     (1)

    void space_partition<StateType, OperatorType> (const space_partition<StateType,
   OperatorType> & )                                                                                                                     (2)

(1)Perform Phase I of the automatic partition algorithm


Parameters
----------

 * **st**: Sample many-body state to be used internally by the algorithm

 * **H**: Hamiltonian as an imperative operator

 * **store_matrix_elements**: Should we store the non-vanishing matrix elements of the Hamiltonian?


Documentation
-------------


   Partitions a Hilbert space into invariant subspaces of the Hamiltonian.



(2)Copy-constructor