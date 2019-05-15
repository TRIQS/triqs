..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__hilbert_space__sub_hilbert_space:

triqs::hilbert_space::sub_hilbert_space
=======================================

Defined in header <*triqs/hilbert_space/hilbert_space.hpp*>

.. code-block:: c

     class sub_hilbert_space

Hilbert subspace, as an ordered set of basis Fock states.


  Subspaces carry an integer index, which allows them to be destinguished as parts of a full Hilbert space.

  Subspaces carry an integer index, which allows them to be destinguished as parts of a full Hilbert space.

Member functions
----------------

+--------------------------------------------------------------------+--------------------------------------------------------------------+
| :ref:`constructor <sub_hilbert_space_constructor>`                 | Construct an empty Hilbert subspace                                |
+--------------------------------------------------------------------+--------------------------------------------------------------------+
| :ref:`add_fock_state <sub_hilbert_space_add_fock_state>`           | Add a Fock state to the Hilbert space basis                        |
+--------------------------------------------------------------------+--------------------------------------------------------------------+
| :ref:`size <sub_hilbert_space_size>`                               | Return the total number of the fermionic Fock states in this space |
+--------------------------------------------------------------------+--------------------------------------------------------------------+
| :ref:`operator== <sub_hilbert_space_operator==>`                   | Check two Hilbert subspaces for equality                           |
+--------------------------------------------------------------------+--------------------------------------------------------------------+
| :ref:`operator!= <sub_hilbert_space_operator!=>`                   | Check two Hilbert subspaces for inequality                         |
+--------------------------------------------------------------------+--------------------------------------------------------------------+
| :ref:`get_state_index <sub_hilbert_space_get_state_index>`         | Find the index of a given Fock state within this subspace          |
+--------------------------------------------------------------------+--------------------------------------------------------------------+
| :ref:`has_state <sub_hilbert_space_has_state>`                     | Check if a given Fock state belongs to this subspace               |
+--------------------------------------------------------------------+--------------------------------------------------------------------+
| :ref:`get_fock_state <sub_hilbert_space_get_fock_state>`           | Return the `i`-th basis element as a Fock state                    |
+--------------------------------------------------------------------+--------------------------------------------------------------------+
| :ref:`get_all_fock_states <sub_hilbert_space_get_all_fock_states>` | Return all basis Fock states in this subspace as `std::vector`     |
+--------------------------------------------------------------------+--------------------------------------------------------------------+
| :ref:`get_index <sub_hilbert_space_get_index>`                     | Return the index of this subspace within the full Hilbert space    |
+--------------------------------------------------------------------+--------------------------------------------------------------------+
| :ref:`set_index <sub_hilbert_space_set_index>`                     | Set the index of this subspace within the full Hilbert space       |
+--------------------------------------------------------------------+--------------------------------------------------------------------+
| :ref:`hdf5_scheme <sub_hilbert_space_hdf5_scheme>`                 | Return name of the HDF5 scheme                                     |
+--------------------------------------------------------------------+--------------------------------------------------------------------+

.. toctree::
    :hidden:

    sub_hilbert_space/constructor
    sub_hilbert_space/add_fock_state
    sub_hilbert_space/size
    sub_hilbert_space/operator==
    sub_hilbert_space/operator!=
    sub_hilbert_space/get_state_index
    sub_hilbert_space/has_state
    sub_hilbert_space/get_fock_state
    sub_hilbert_space/get_all_fock_states
    sub_hilbert_space/get_index
    sub_hilbert_space/set_index
    sub_hilbert_space/hdf5_scheme


Non Member functions
--------------------

+----------------------------------------------+--------------------------------------------+
| :ref:`h5_write <sub_hilbert_space_h5_write>` | Write a Hilbert subspace to an HDF5 group  |
+----------------------------------------------+--------------------------------------------+
| :ref:`h5_read <sub_hilbert_space_h5_read>`   | Read a Hilbert subspace from an HDF5 group |
+----------------------------------------------+--------------------------------------------+

.. toctree::
    :hidden:

    sub_hilbert_space/h5_write
    sub_hilbert_space/h5_read