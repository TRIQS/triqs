..
   Generated automatically by cpp2rst

.. highlight:: c


.. code-block:: c

    #include <triqs/hilbert_space/fundamental_operator_set.hpp>

.. _fundamental_operator_set:

fundamental_operator_set
==========================

This class represents an ordered set of **indices** of the canonical operators (see [[many_body_operator]]) used to build the Fock states.


 Every element of the set is an arbitrarily long sequence of integers/strings (types can be mixed within one sequence).
 The elements maintain the order they are inserted in


**Synopsis**:

.. code-block:: c

    class fundamental_operator_set;

 Every element of the set is an arbitrarily long sequence of integers/strings (types can be mixed within one sequence).
 The elements maintain the order they are inserted in


Member types
------------

+----------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Member type    | Comment                                                                                                                                                                                                          |
+================+==================================================================================================================================================================================================================+
| indices_t      | /// Sequence of indices (`std::vector` of int/string variant objects)                                                                                                                                            |
+----------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| data_t         | /// The basic container `std::vector<indices_t>`                                                                                                                                                                 |
+----------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| const_iterator | /// Constant bidirectional iterator over all stored index sequences. For an iterator `it`, `it->index` gives the `indices_t` object pointed by this iterator, and `it->linear_index` is its position in the set. |
+----------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+


Member functions
----------------

+-------------------------------------------------------------------------------+-----------------------------------------------------------------------------+
| Member function                                                               | Comment                                                                     |
+===============================================================================+=============================================================================+
| :ref:`insert_from_indices_t <fundamental_operator_set_insert_from_indices_t>` | Insert a new index sequence given as `indices_t`                            |
+-------------------------------------------------------------------------------+-----------------------------------------------------------------------------+
| :ref:`insert <fundamental_operator_set_insert>`                               | Insert a new index sequence given as multiple `int`/`std::string` arguments |
+-------------------------------------------------------------------------------+-----------------------------------------------------------------------------+
| :ref:`size <fundamental_operator_set_size>`                                   | Number of elements in this set                                              |
+-------------------------------------------------------------------------------+-----------------------------------------------------------------------------+
| :ref:`has_indices <fundamental_operator_set_has_indices>`                     | Check if a given index sequence is in this set                              |
+-------------------------------------------------------------------------------+-----------------------------------------------------------------------------+
| :ref:`operator[] <fundamental_operator_set_operator[]>`                       | Request position of a given index sequence                                  |
+-------------------------------------------------------------------------------+-----------------------------------------------------------------------------+
| :ref:`operator== != <fundamental_operator_set_operator== !=>`                 | Comparison with another fundamental operator set                            |
+-------------------------------------------------------------------------------+-----------------------------------------------------------------------------+
| :ref:`data <fundamental_operator_set_data>`                                   | Return the data vector: `v[int]` -> `indices_t`                             |
+-------------------------------------------------------------------------------+-----------------------------------------------------------------------------+
| :ref:`begin <fundamental_operator_set_begin>`                                 | Return `const_iterator` to the first element of this set                    |
+-------------------------------------------------------------------------------+-----------------------------------------------------------------------------+
| :ref:`end <fundamental_operator_set_end>`                                     | Return `const_iterator` to the past-the-end element of this set             |
+-------------------------------------------------------------------------------+-----------------------------------------------------------------------------+
| :ref:`cbegin <fundamental_operator_set_cbegin>`                               | Equivalent to :ref:`fundamental_operator_set_begin`                         |
+-------------------------------------------------------------------------------+-----------------------------------------------------------------------------+
| :ref:`cend <fundamental_operator_set_cend>`                                   | Equivalent to :ref:`fundamental_operator_set_end`                           |
+-------------------------------------------------------------------------------+-----------------------------------------------------------------------------+
| :ref:`constructor <fundamental_operator_set_constructor>`                     | Construct an empty set                                                      |
+-------------------------------------------------------------------------------+-----------------------------------------------------------------------------+

.. toctree::
    :hidden:

    fundamental_operator_set/insert_from_indices_t
    fundamental_operator_set/insert
    fundamental_operator_set/size
    fundamental_operator_set/has_indices
    fundamental_operator_set/operator[]
    fundamental_operator_set/operator== !=
    fundamental_operator_set/data
    fundamental_operator_set/begin
    fundamental_operator_set/end
    fundamental_operator_set/cbegin
    fundamental_operator_set/cend
    fundamental_operator_set/constructor


Non Member functions
--------------------

+-------------------------------------------------------------------------+-------------------------------------+
| Non member function                                                     | Comment                             |
+=========================================================================+=====================================+
| :ref:`h5_write_attribute <fundamental_operator_set_h5_write_attribute>` | Write this set as an HDF5 attribute |
+-------------------------------------------------------------------------+-------------------------------------+
| :ref:`h5_read_attribute <fundamental_operator_set_h5_read_attribute>`   | Read a set from an HDF5 attribute   |
+-------------------------------------------------------------------------+-------------------------------------+

.. toctree::
    :hidden:

    fundamental_operator_set/h5_write_attribute
    fundamental_operator_set/h5_read_attribute