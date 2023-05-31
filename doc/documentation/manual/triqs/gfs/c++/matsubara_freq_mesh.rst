..
   Generated automatically using the command:
   c++2doc.py -N triqs all_triqs.hpp

.. highlight:: c

.. _gf_mesh_imfreq:


mesh::imfreq
=================

**Synopsis**:

.. code-block:: c

      class mesh::imfreq;

Matsubara frequency mesh

 The class `matsubara_freq_mesh` is the implementation of a mesh of fermionic (:math:`\frac{(2n+1)\pi}{\beta}`) or bosonic (:math:`\frac{2n\pi}{\beta}`) Matsubara frequencies.

 The mesh can span either only positive frequencies or both positive and negative frequencies (which is necessary for complex functions :math:`G(\tau)`).

.. figure:: matsubara_freq_mesh.png
   :alt:  Pictorial representation of ``mesh::imfreq({beta, Fermion/Boson, 3, all_frequencies/positive_frequencies_only})``. See
   :align: center

   Pictorial representation of ``mesh::imfreq({beta, Fermion/Boson, 3, all_frequencies/positive_frequencies_only})``. See



Member types
-----------------

+-------------------------+-----------------------------------------------------+
| Member type             | Comment                                             |
+=========================+=====================================================+
| domain_t                | type of the domain: matsubara_freq_domain           |
+-------------------------+-----------------------------------------------------+
| index_t                 | type of the Matsubara index $n$ (as in $i\omega_n$) |
+-------------------------+-----------------------------------------------------+
| linear_index_t          | type of the linear index                            |
+-------------------------+-----------------------------------------------------+
| default_interpol_policy |                                                     |
+-------------------------+-----------------------------------------------------+
| domain_pt_t             | type of the domain point                            |
+-------------------------+-----------------------------------------------------+
| mesh_point_t            | Type of the mesh point                              |
+-------------------------+-----------------------------------------------------+
| const_iterator          | Iterating on all the points...                      |
+-------------------------+-----------------------------------------------------+

Member functions
-----------------

+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| Member function                                                | Comment                                                                                  |
+================================================================+==========================================================================================+
| :ref:`constructor <mesh::imfreq_constructor>`                  | constructor                                                                              |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`domain <mesh::imfreq_domain>`                            | The corresponding domain                                                                 |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`size <mesh::imfreq_size>`                                | Size (linear) of the mesh of the window                                                  |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`full_size <mesh::imfreq_full_size>`                      | Size (linear) of the mesh of the window                                                  |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`index_to_point <mesh::imfreq_index_to_point>`            | From an index of a point in the mesh, returns the corresponding point in the domain      |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`index_to_linear <mesh::imfreq_index_to_linear>`          | Flatten the index in the positive linear index for memory storage (almost trivial here). |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`linear_to_index <mesh::imfreq_linear_to_index>`          | Reverse of index_to_linear                                                               |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`first_index <mesh::imfreq_first_index>`                  | first Matsubara index                                                                    |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`last_index <mesh::imfreq_last_index>`                    | last Matsubara index                                                                     |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`first_index_window <mesh::imfreq_first_index_window>`    | first Matsubara index of the window                                                      |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`last_index_window <mesh::imfreq_last_index_window>`      | last Matsubara index of the window                                                       |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`positive_only <mesh::imfreq_positive_only>`              | Is the mesh only for positive omega_n (G(tau) real))                                     |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`operator[] <mesh::imfreq_operator[]>`                    | Accessing a point of the mesh from its index                                             |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`is_within_boundary <mesh::imfreq_is_within_boundary>`    | Is the point in mesh ?                                                                   |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+

.. toctree::

   :hidden:

   mesh::imfreq/constructor
   mesh::imfreq/domain
   mesh::imfreq/size
   mesh::imfreq/full_size
   mesh::imfreq/index_to_point
   mesh::imfreq/index_to_linear
   mesh::imfreq/linear_to_index
   mesh::imfreq/first_index
   mesh::imfreq/last_index
   mesh::imfreq/first_index_window
   mesh::imfreq/last_index_window
   mesh::imfreq/positive_only
   mesh::imfreq/operator[]
   mesh::imfreq/is_within_boundary

Non Member functions
-----------------------

+--------------------------------------------------+----------------------------------------+
| Non member function                              | Comment                                |
+==================================================+========================================+
| :ref:`mpi_scatter <mesh::imfreq_mpi_scatter>`    | Scatter a mesh over the communicator c |
+--------------------------------------------------+----------------------------------------+
| :ref:`mpi_gather <mesh::imfreq_mpi_gather>`      | Opposite of scatter                    |
+--------------------------------------------------+----------------------------------------+
| :ref:`h5_write <mesh::imfreq_h5_write>`          | Write into HDF5                        |
+--------------------------------------------------+----------------------------------------+
| :ref:`h5_read <mesh::imfreq_h5_read>`            | Read from HDF5                         |
+--------------------------------------------------+----------------------------------------+

.. toctree::

   :hidden:

   mesh::imfreq/mpi_scatter
   mesh::imfreq/mpi_gather
   mesh::imfreq/h5_write
   mesh::imfreq/h5_read
