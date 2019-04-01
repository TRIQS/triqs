..
   Generated automatically by cpp2rst

.. highlight:: c


.. _space_partition:

space_partition
=================

Implementation of the automatic partitioning algorithm


  Partitions a Hilbert space into a set of subspaces invariant under action of a given Hermitian operator (Hamiltonian).
  Optionally modifies the partition so that a given operator generates only one-to-one connections between the subspaces.
  For a detailed description of the algorithm see
  `Computer Physics Communications 200, March 2016, 274-284 <http:dx.doi.org/10.1016/j.cpc.2015.10.023>`_ (section 4.2).



**Synopsis**:

.. code-block:: c

    template<typename StateType, typename OperatorType> class space_partition;

  Partitions a Hilbert space into a set of subspaces invariant under action of a given Hermitian operator (Hamiltonian).
  Optionally modifies the partition so that a given operator generates only one-to-one connections between the subspaces.
  For a detailed description of the algorithm see
  `Computer Physics Communications 200, March 2016, 274-284 <http:dx.doi.org/10.1016/j.cpc.2015.10.023>`_ (section 4.2).



Template parameters
-------------------

 * **StateType**: Many-body state type, must model [[statevector_concept]]

 * **OperatorType**: Imperative operator type, must provide `StateType operator()(StateType const&)`


Member types
------------

+----------------------+-----------------------------------------------------------------------------------------------------+
| Member type          | Comment                                                                                             |
+======================+=====================================================================================================+
| index_t              | /// Index of a basis Fock state/subspace                                                            |
+----------------------+-----------------------------------------------------------------------------------------------------+
| state_t              | /// Accessor to `StateType` template parameter                                                      |
+----------------------+-----------------------------------------------------------------------------------------------------+
| operator_t           | /// Accessor to `OperatorType` template parameter                                                   |
+----------------------+-----------------------------------------------------------------------------------------------------+
| amplitude_t          | /// Amplitude type of the state                                                                     |
+----------------------+-----------------------------------------------------------------------------------------------------+
| block_mapping_t      | /// Connections between subspaces represented as a set of (from-index,to-index) pair                |
+----------------------+-----------------------------------------------------------------------------------------------------+
| matrix_element_map_t | /// Non-zero matrix elements of an operator represented as a mapping (from-state,to-state) -> value |
+----------------------+-----------------------------------------------------------------------------------------------------+


Member functions
----------------

+------------------------------------------------------------------+-------------------------------------------------------------------------+
| Member function                                                  | Comment                                                                 |
+==================================================================+=========================================================================+
| :ref:`merge_subspaces <space_partition_merge_subspaces>`         | Perform Phase II of the automatic partition algorithm                   |
+------------------------------------------------------------------+-------------------------------------------------------------------------+
| :ref:`n_subspaces <space_partition_n_subspaces>`                 | Return the number of subspaces in the partition                         |
+------------------------------------------------------------------+-------------------------------------------------------------------------+
| :ref:`lookup_basis_state <space_partition_lookup_basis_state>`   | Find what invariant subspace a given Fock state belongs to              |
+------------------------------------------------------------------+-------------------------------------------------------------------------+
| :ref:`get_matrix_elements <space_partition_get_matrix_elements>` | Access to matrix elements of the Hamiltonian                            |
+------------------------------------------------------------------+-------------------------------------------------------------------------+
| :ref:`find_mappings <space_partition_find_mappings>`             | Find all subspace-to-subspace connections generated by a given operator |
+------------------------------------------------------------------+-------------------------------------------------------------------------+
| :ref:`constructor <space_partition_constructor>`                 | Perform Phase I of the automatic partition algorithm                    |
+------------------------------------------------------------------+-------------------------------------------------------------------------+

.. toctree::
    :hidden:

    space_partition/merge_subspaces
    space_partition/n_subspaces
    space_partition/lookup_basis_state
    space_partition/get_matrix_elements
    space_partition/find_mappings
    space_partition/constructor


Non Member functions
--------------------

+------------------------------------------+-----------------------------------------------------------------------------+
| Non member function                      | Comment                                                                     |
+==========================================+=============================================================================+
| :ref:`foreach <space_partition_foreach>` | Apply a callable object to all basis Fock states in a given space partition |
+------------------------------------------+-----------------------------------------------------------------------------+

.. toctree::
    :hidden:

    space_partition/foreach