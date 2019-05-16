..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__hilbert_space__fundamental_operator_set:

triqs::hilbert_space::fundamental_operator_set
==============================================

*#include <triqs/hilbert_space/fundamental_operator_set.hpp>*

.. rst-class:: cppsynopsis

      class  :red:`fundamental_operator_set`

This class represents an ordered set of **indices** of the canonical operators (see [[many_body_operator]]) used to build the Fock states.
Every element of the set is an arbitrarily long sequence of integers/strings (types can be mixed within one sequence).
The elements maintain the order they are inserted in


Member types
------------

+----------------+-----------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| indices_t      | indices_t                                                 |  Sequence of indices (`std::vector` of int/string variant objects)                                                                                                                                            |
+----------------+-----------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| data_t         | std::vector<indices_t>                                    |  The basic container `std::vector<indices_t>`                                                                                                                                                                 |
+----------------+-----------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| const_iterator | triqs::utility::dressed_iterator<_enum_iterator, _cdress> |  Constant bidirectional iterator over all stored index sequences. For an iterator `it`, `it->index` gives the `indices_t` object pointed by this iterator, and `it->linear_index` is its position in the set. |
+----------------+-----------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+


Member functions
----------------

+------------------------------------------------------------------------------------------------------+--+
| :ref:`(constructor) <triqs__hilbert_space__fundamental_operator_set__constructor>`                   |  |
+------------------------------------------------------------------------------------------------------+--+
| :ref:`insert_from_indices_t <triqs__hilbert_space__fundamental_operator_set__insert_from_indices_t>` |  |
+------------------------------------------------------------------------------------------------------+--+
| :ref:`insert <triqs__hilbert_space__fundamental_operator_set__insert>`                               |  |
+------------------------------------------------------------------------------------------------------+--+
| :ref:`size <triqs__hilbert_space__fundamental_operator_set__size>`                                   |  |
+------------------------------------------------------------------------------------------------------+--+
| :ref:`has_indices <triqs__hilbert_space__fundamental_operator_set__has_indices>`                     |  |
+------------------------------------------------------------------------------------------------------+--+
| :ref:`operator[] <triqs__hilbert_space__fundamental_operator_set__operator[]>`                       |  |
+------------------------------------------------------------------------------------------------------+--+
| :ref:`operator== <triqs__hilbert_space__fundamental_operator_set__operator==>`                       |  |
+------------------------------------------------------------------------------------------------------+--+
| :ref:`data <triqs__hilbert_space__fundamental_operator_set__data>`                                   |  |
+------------------------------------------------------------------------------------------------------+--+
| :ref:`begin <triqs__hilbert_space__fundamental_operator_set__begin>`                                 |  |
+------------------------------------------------------------------------------------------------------+--+
| :ref:`end <triqs__hilbert_space__fundamental_operator_set__end>`                                     |  |
+------------------------------------------------------------------------------------------------------+--+
| :ref:`cbegin <triqs__hilbert_space__fundamental_operator_set__cbegin>`                               |  |
+------------------------------------------------------------------------------------------------------+--+
| :ref:`cend <triqs__hilbert_space__fundamental_operator_set__cend>`                                   |  |
+------------------------------------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    fundamental_operator_set/constructor
    fundamental_operator_set/insert_from_indices_t
    fundamental_operator_set/insert
    fundamental_operator_set/size
    fundamental_operator_set/has_indices
    fundamental_operator_set/operator[]
    fundamental_operator_set/operator==
    fundamental_operator_set/data
    fundamental_operator_set/begin
    fundamental_operator_set/end
    fundamental_operator_set/cbegin
    fundamental_operator_set/cend


Non Member functions
--------------------

+------------------------------------------------------------------------------------------------+--+
| :ref:`h5_write_attribute <triqs__hilbert_space__fundamental_operator_set__h5_write_attribute>` |  |
+------------------------------------------------------------------------------------------------+--+
| :ref:`h5_read_attribute <triqs__hilbert_space__fundamental_operator_set__h5_read_attribute>`   |  |
+------------------------------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    fundamental_operator_set/h5_write_attribute
    fundamental_operator_set/h5_read_attribute