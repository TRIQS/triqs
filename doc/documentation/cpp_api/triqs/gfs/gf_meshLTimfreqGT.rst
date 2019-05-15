..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__gfs__gf_meshLTimfreqGT:

triqs::gfs::gf_mesh<imfreq>
===========================

Defined in header <**>

.. code-block:: c

     class gf_mesh<imfreq>

Matsubara frequency mesh


 The class `matsubara_freq_mesh` is the implementation of a mesh of fermionic (:math:`\frac{(2n+1)\pi}{\beta}`) or bosonic (:math:`\frac{2n\pi}{\beta}`) Matsubara frequencies.

 The mesh can span either only positive frequencies or both positive and negative frequencies (which is necessary for complex functions :math:`G(\tau)`).

 The class `matsubara_freq_mesh` is the implementation of a mesh of fermionic (:math:`\frac{(2n+1)\pi}{\beta}`) or bosonic (:math:`\frac{2n\pi}{\beta}`) Matsubara frequencies.

 The mesh can span either only positive frequencies or both positive and negative frequencies (which is necessary for complex functions :math:`G(\tau)`).

.. figure:: matsubara_freq_mesh.png
   :alt:  Pictorial representation of ``gf_mesh<imfreq>({beta, Fermion/Boson, 3, all_frequencies/positive_frequencies_only})``. See :ref:`constructor <gf_mesh<imfreq>_constructor>` for more details.
   :align: center

Pictorial representation of ``gf_mesh<imfreq>({beta, Fermion/Boson, 3, all_frequencies/positive_frequencies_only})``. See :ref:`constructor <gf_mesh<imfreq>_constructor>` for more details.


Member types
------------

+----------------+-------------------------------------+-------------------------------------------------------------------------------------------+
| domain_t       | matsubara_domain<true>              | type of the domain: matsubara_domain<true>                                                |
+----------------+-------------------------------------+-------------------------------------------------------------------------------------------+
| index_t        | _long                               | type of the Matsubara index :math:`n` (as in :math:`i\omega_n`)
    using index_t = long; |
+----------------+-------------------------------------+-------------------------------------------------------------------------------------------+
| linear_index_t | long                                | type of the linear index                                                                  |
+----------------+-------------------------------------+-------------------------------------------------------------------------------------------+
| domain_pt_t    | typename domain_t::point_t          | type of the domain point                                                                  |
+----------------+-------------------------------------+-------------------------------------------------------------------------------------------+
| var_t          | imfreq                              |                                                                                           |
+----------------+-------------------------------------+-------------------------------------------------------------------------------------------+
| mesh_point_t   | mesh_point<gf_mesh<imfreq> >        |  Type of the mesh point                                                                   |
+----------------+-------------------------------------+-------------------------------------------------------------------------------------------+
| const_iterator | mesh_pt_generator<gf_mesh<imfreq> > |  Iterating on all the points...                                                           |
+----------------+-------------------------------------+-------------------------------------------------------------------------------------------+


Member functions
----------------

+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`constructor <gf_meshLTimfreqGT_constructor>`                         | default constructor                                                                      |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`set_tail_fit_parameters <gf_meshLTimfreqGT_set_tail_fit_parameters>` | Adjust the parameters for the tail-fitting                                               |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`get_tail_fitter <gf_meshLTimfreqGT_get_tail_fitter>`                 | The tail fitter is mutable, even if the mesh is immutable to cache some data             |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`operator== <gf_meshLTimfreqGT_operator==>`                           | -------------------- Comparisons -------------------                                     |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`operator!= <gf_meshLTimfreqGT_operator!=>`                           |                                                                                          |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`domain <gf_meshLTimfreqGT_domain>`                                   | The corresponding domain                                                                 |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`size <gf_meshLTimfreqGT_size>`                                       | Size (linear) of the mesh                                                                |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`size_of_components <gf_meshLTimfreqGT_size_of_components>`           |                                                                                          |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`index_to_point <gf_meshLTimfreqGT_index_to_point>`                   | From an index of a point in the mesh, returns the corresponding point in the domain      |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`index_to_linear <gf_meshLTimfreqGT_index_to_linear>`                 | Flatten the index in the positive linear index for memory storage (almost trivial here). |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`linear_to_index <gf_meshLTimfreqGT_linear_to_index>`                 | Reverse of index_to_linear                                                               |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`first_index <gf_meshLTimfreqGT_first_index>`                         | first Matsubara index                                                                    |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`last_index <gf_meshLTimfreqGT_last_index>`                           | last Matsubara index                                                                     |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`positive_only <gf_meshLTimfreqGT_positive_only>`                     | Is the mesh only for positive omega_n (G(tau) real))                                     |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`get_positive_freq <gf_meshLTimfreqGT_get_positive_freq>`             | -------------------- Get the grid for positive freq only -------------------             |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`omega_max <gf_meshLTimfreqGT_omega_max>`                             | maximum freq of the mesh                                                                 |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`operator[] <gf_meshLTimfreqGT_operator[]>`                           | Accessing a point of the mesh from its index                                             |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`begin <gf_meshLTimfreqGT_begin>`                                     |                                                                                          |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`end <gf_meshLTimfreqGT_end>`                                         |                                                                                          |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`cbegin <gf_meshLTimfreqGT_cbegin>`                                   |                                                                                          |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`cend <gf_meshLTimfreqGT_cend>`                                       |                                                                                          |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`is_within_boundary <gf_meshLTimfreqGT_is_within_boundary>`           | Is the point in mesh ?                                                                   |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`get_interpolation_data <gf_meshLTimfreqGT_get_interpolation_data>`   | For multivar evaluation                                                                  |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`evaluate <gf_meshLTimfreqGT_evaluate>`                               | For one var evaluation                                                                   |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`hdf5_scheme <gf_meshLTimfreqGT_hdf5_scheme>`                         | -------------------- HDF5 -------------------                                            |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+
| :ref:`serialize <gf_meshLTimfreqGT_serialize>`                             |                                                                                          |
+----------------------------------------------------------------------------+------------------------------------------------------------------------------------------+

.. toctree::
    :hidden:

    gf_meshLTimfreqGT/constructor
    gf_meshLTimfreqGT/set_tail_fit_parameters
    gf_meshLTimfreqGT/get_tail_fitter
    gf_meshLTimfreqGT/operator==
    gf_meshLTimfreqGT/operator!=
    gf_meshLTimfreqGT/domain
    gf_meshLTimfreqGT/size
    gf_meshLTimfreqGT/size_of_components
    gf_meshLTimfreqGT/index_to_point
    gf_meshLTimfreqGT/index_to_linear
    gf_meshLTimfreqGT/linear_to_index
    gf_meshLTimfreqGT/first_index
    gf_meshLTimfreqGT/last_index
    gf_meshLTimfreqGT/positive_only
    gf_meshLTimfreqGT/get_positive_freq
    gf_meshLTimfreqGT/omega_max
    gf_meshLTimfreqGT/operator[]
    gf_meshLTimfreqGT/begin
    gf_meshLTimfreqGT/end
    gf_meshLTimfreqGT/cbegin
    gf_meshLTimfreqGT/cend
    gf_meshLTimfreqGT/is_within_boundary
    gf_meshLTimfreqGT/get_interpolation_data
    gf_meshLTimfreqGT/evaluate
    gf_meshLTimfreqGT/hdf5_scheme
    gf_meshLTimfreqGT/serialize


Non Member functions
--------------------

+------------------------------------------------------+-----------------+
| :ref:`operator\<\< <gf_meshLTimfreqGT_operatorLTLT>` |                 |
+------------------------------------------------------+-----------------+
| :ref:`h5_write <gf_meshLTimfreqGT_h5_write>`         | Write into HDF5 |
+------------------------------------------------------+-----------------+
| :ref:`h5_read <gf_meshLTimfreqGT_h5_read>`           | Read from HDF5  |
+------------------------------------------------------+-----------------+
| :ref:`boost <gf_meshLTimfreqGT_boost>`               |                 |
+------------------------------------------------------+-----------------+

.. toctree::
    :hidden:

    gf_meshLTimfreqGT/operator<<
    gf_meshLTimfreqGT/h5_write
    gf_meshLTimfreqGT/h5_read
    gf_meshLTimfreqGT/boost