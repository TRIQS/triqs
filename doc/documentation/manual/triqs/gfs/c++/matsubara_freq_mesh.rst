..
   Generated automatically using the command:
   c++2doc.py -N triqs all_triqs.hpp

.. highlight:: c

.. _gf_mesh_imfreq:


gf_mesh<imfreq>
=================

**Synopsis**:

.. code-block:: c

      class gf_mesh<imfreq>;

Matsubara frequency mesh

 The class `matsubara_freq_mesh` is the implementation of a mesh of fermionic (:math:`\frac{(2n+1)\pi}{\beta}`) or bosonic (:math:`\frac{2n\pi}{\beta}`) Matsubara frequencies.

 The mesh can span either only positive frequencies or both positive and negative frequencies (which is necessary for complex functions :math:`G(\tau)`).

.. figure:: matsubara_freq_mesh.png
   :alt:  Pictorial representation of ``gf_mesh<imfreq>({beta, Fermion/Boson, 3, all_frequencies/positive_frequencies_only})``. See
   :align: center

   Pictorial representation of ``gf_mesh<imfreq>({beta, Fermion/Boson, 3, all_frequencies/positive_frequencies_only})``. See



Member types
-----------------

+-------------------------+-----------------------------------------------------+
| Member type             | Comment                                             |
+=========================+=====================================================+
| domain_t                | type of the domain: matsubara_domain<true>          |
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
| :ref:`constructor <gf_mesh<imfreq>_constructor>`               | constructor                                                                              |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`domain <gf_mesh<imfreq>_domain>`                         | The corresponding domain                                                                 |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`size <gf_mesh<imfreq>_size>`                             | Size (linear) of the mesh of the window                                                  |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`full_size <gf_mesh<imfreq>_full_size>`                   | Size (linear) of the mesh of the window                                                  |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`index_to_point <gf_mesh<imfreq>_index_to_point>`         | From an index of a point in the mesh, returns the corresponding point in the domain      |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`index_to_linear <gf_mesh<imfreq>_index_to_linear>`       | Flatten the index in the positive linear index for memory storage (almost trivial here). |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`linear_to_index <gf_mesh<imfreq>_linear_to_index>`       | Reverse of index_to_linear                                                               |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`first_index <gf_mesh<imfreq>_first_index>`               | first Matsubara index                                                                    |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`last_index <gf_mesh<imfreq>_last_index>`                 | last Matsubara index                                                                     |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`first_index_window <gf_mesh<imfreq>_first_index_window>` | first Matsubara index of the window                                                      |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`last_index_window <gf_mesh<imfreq>_last_index_window>`   | last Matsubara index of the window                                                       |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`positive_only <gf_mesh<imfreq>_positive_only>`           | Is the mesh only for positive omega_n (G(tau) real))                                     |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`operator[] <gf_mesh<imfreq>_operator[]>`                 | Accessing a point of the mesh from its index                                             |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`is_within_boundary <gf_mesh<imfreq>_is_within_boundary>` | Is the point in mesh ?                                                                   |
+----------------------------------------------------------------+------------------------------------------------------------------------------------------+

.. toctree::

   :hidden:

   gf_mesh<imfreq>/constructor
   gf_mesh<imfreq>/domain
   gf_mesh<imfreq>/size
   gf_mesh<imfreq>/full_size
   gf_mesh<imfreq>/index_to_point
   gf_mesh<imfreq>/index_to_linear
   gf_mesh<imfreq>/linear_to_index
   gf_mesh<imfreq>/first_index
   gf_mesh<imfreq>/last_index
   gf_mesh<imfreq>/first_index_window
   gf_mesh<imfreq>/last_index_window
   gf_mesh<imfreq>/positive_only
   gf_mesh<imfreq>/operator[]
   gf_mesh<imfreq>/is_within_boundary

Non Member functions
-----------------------

+--------------------------------------------------+----------------------------------------+
| Non member function                              | Comment                                |
+==================================================+========================================+
| :ref:`mpi_scatter <gf_mesh<imfreq>_mpi_scatter>` | Scatter a mesh over the communicator c |
+--------------------------------------------------+----------------------------------------+
| :ref:`mpi_gather <gf_mesh<imfreq>_mpi_gather>`   | Opposite of scatter                    |
+--------------------------------------------------+----------------------------------------+
| :ref:`h5_write <gf_mesh<imfreq>_h5_write>`       | Write into HDF5                        |
+--------------------------------------------------+----------------------------------------+
| :ref:`h5_read <gf_mesh<imfreq>_h5_read>`         | Read from HDF5                         |
+--------------------------------------------------+----------------------------------------+

.. toctree::

   :hidden:

   gf_mesh<imfreq>/mpi_scatter
   gf_mesh<imfreq>/mpi_gather
   gf_mesh<imfreq>/h5_write
   gf_mesh<imfreq>/h5_read
