..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__hilbert_space__space_partition:

triqs::hilbert_space::space_partition
=====================================

*#include <triqs/hilbert_space.hpp>*

.. rst-class:: cppsynopsis

     template<typename StateType, typename OperatorType> class  :red:`space_partition`

Implementation of the automatic partitioning algorithm

 Partitions a Hilbert space into a set of subspaces invariant under action of a given Hermitian operator (Hamiltonian).
 Optionally modifies the partition so that a given operator generates only one-to-one connections between the subspaces.
 For a detailed description of the algorithm see
 `Computer Physics Communications 200, March 2016, 274-284 <http://dx.doi.org/10.1016/j.cpc.2015.10.023>`_ (section 4.2).


Template parameters
-------------------

 * **StateType**: Many-body state type, must model [[statevector_concept]]

 * **OperatorType**: Imperative operator type, must provide `StateType operator()(StateType const&)`


Member types
------------

+----------------------+---------------------------------------------------------------------+--------------------------------------------------------------------------------------------------+
| index_t              | uint32_t                                                            |  Index of a basis Fock state/subspace                                                            |
+----------------------+---------------------------------------------------------------------+--------------------------------------------------------------------------------------------------+
| state_t              | StateType                                                           |  Accessor to `StateType` template parameter                                                      |
+----------------------+---------------------------------------------------------------------+--------------------------------------------------------------------------------------------------+
| operator_t           | OperatorType                                                        |  Accessor to `OperatorType` template parameter                                                   |
+----------------------+---------------------------------------------------------------------+--------------------------------------------------------------------------------------------------+
| amplitude_t          | typename state_t::value_type                                        |  Amplitude type of the state                                                                     |
+----------------------+---------------------------------------------------------------------+--------------------------------------------------------------------------------------------------+
| block_mapping_t      | std::set<std::pair<index_t, index_t> >                              |  Connections between subspaces represented as a set of (from-index,to-index) pair                |
+----------------------+---------------------------------------------------------------------+--------------------------------------------------------------------------------------------------+
| matrix_element_map_t | std::map<std::pair<index_t, index_t>, typename state_t::value_type> |  Non-zero matrix elements of an operator represented as a mapping (from-state,to-state) -> value |
+----------------------+---------------------------------------------------------------------+--------------------------------------------------------------------------------------------------+


Member functions
----------------

+-----------------------------------------------------------------------------------------+--+
| :ref:`(constructor) <triqs__hilbert_space__space_partition__constructor>`               |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`merge_subspaces <triqs__hilbert_space__space_partition__merge_subspaces>`         |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`n_subspaces <triqs__hilbert_space__space_partition__n_subspaces>`                 |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`lookup_basis_state <triqs__hilbert_space__space_partition__lookup_basis_state>`   |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`get_matrix_elements <triqs__hilbert_space__space_partition__get_matrix_elements>` |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`find_mappings <triqs__hilbert_space__space_partition__find_mappings>`             |  |
+-----------------------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    space_partition/constructor
    space_partition/merge_subspaces
    space_partition/n_subspaces
    space_partition/lookup_basis_state
    space_partition/get_matrix_elements
    space_partition/find_mappings


Non Member functions
--------------------

+-----------------------------------------------------------------+--+
| :ref:`foreach <triqs__hilbert_space__space_partition__foreach>` |  |
+-----------------------------------------------------------------+--+

.. toctree::
    :hidden:

    space_partition/foreach