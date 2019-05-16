..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf_meshLTimfreqGT:

triqs::gfs::gf_mesh<imfreq>
===========================

*#include <triqs/gfs.hpp>*

.. rst-class:: cppsynopsis

      class  :red:`gf_mesh<imfreq>`

Matsubara frequency mesh
The class `matsubara_freq_mesh` is the implementation of a mesh of fermionic (:math:`\frac{(2n+1)\pi}{\beta}`) or bosonic (:math:`\frac{2n\pi}{\beta}`) Matsubara frequencies.

The mesh can span either only positive frequencies or both positive and negative frequencies (which is necessary for complex functions :math:`G(\tau)`).


.. figure:: matsubara_freq_mesh.png
   :alt:  Pictorial representation of ``gf_mesh<imfreq>({beta, Fermion/Boson, 3, all_frequencies/positive_frequencies_only})``. See :ref:`constructor <gf_mesh<imfreq>_constructor>` for more details.
   :align: center

Pictorial representation of ``gf_mesh<imfreq>({beta, Fermion/Boson, 3, all_frequencies/positive_frequencies_only})``. See :ref:`constructor <gf_mesh<imfreq>_constructor>` for more details.


Member types
------------

+----------------+-------------------------------------+---------------------------------------------------------------------------------------------+
| domain_t       | matsubara_domain<true>              | type of the domain: matsubara_domain<true>                                                  |
+----------------+-------------------------------------+---------------------------------------------------------------------------------------------+
| index_t        | _long                               | type of the Matsubara index :math:`n` (as in :math:`i\omega_n`)
    //using index_t = long; |
+----------------+-------------------------------------+---------------------------------------------------------------------------------------------+
| linear_index_t | long                                | type of the linear index                                                                    |
+----------------+-------------------------------------+---------------------------------------------------------------------------------------------+
| domain_pt_t    | typename domain_t::point_t          | type of the domain point                                                                    |
+----------------+-------------------------------------+---------------------------------------------------------------------------------------------+
| var_t          | imfreq                              |                                                                                             |
+----------------+-------------------------------------+---------------------------------------------------------------------------------------------+
| mesh_point_t   | mesh_point<gf_mesh<imfreq> >        |  Type of the mesh point                                                                     |
+----------------+-------------------------------------+---------------------------------------------------------------------------------------------+
| const_iterator | mesh_pt_generator<gf_mesh<imfreq> > |  Iterating on all the points...                                                             |
+----------------+-------------------------------------+---------------------------------------------------------------------------------------------+


Member functions
----------------

+-----------------------------------------------------------------------------------------+--+
| :ref:`(constructor) <triqs__gfs__gf_meshLTimfreqGT__constructor>`                       |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`set_tail_fit_parameters <triqs__gfs__gf_meshLTimfreqGT__set_tail_fit_parameters>` |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`get_tail_fitter <triqs__gfs__gf_meshLTimfreqGT__get_tail_fitter>`                 |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`operator== <triqs__gfs__gf_meshLTimfreqGT__operator==>`                           |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`domain <triqs__gfs__gf_meshLTimfreqGT__domain>`                                   |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`size <triqs__gfs__gf_meshLTimfreqGT__size>`                                       |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`size_of_components <triqs__gfs__gf_meshLTimfreqGT__size_of_components>`           |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`index_to_point <triqs__gfs__gf_meshLTimfreqGT__index_to_point>`                   |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`index_to_linear <triqs__gfs__gf_meshLTimfreqGT__index_to_linear>`                 |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`linear_to_index <triqs__gfs__gf_meshLTimfreqGT__linear_to_index>`                 |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`first_index <triqs__gfs__gf_meshLTimfreqGT__first_index>`                         |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`last_index <triqs__gfs__gf_meshLTimfreqGT__last_index>`                           |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`positive_only <triqs__gfs__gf_meshLTimfreqGT__positive_only>`                     |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`get_positive_freq <triqs__gfs__gf_meshLTimfreqGT__get_positive_freq>`             |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`omega_max <triqs__gfs__gf_meshLTimfreqGT__omega_max>`                             |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`operator[] <triqs__gfs__gf_meshLTimfreqGT__operator[]>`                           |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`is_within_boundary <triqs__gfs__gf_meshLTimfreqGT__is_within_boundary>`           |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`get_interpolation_data <triqs__gfs__gf_meshLTimfreqGT__get_interpolation_data>`   |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`evaluate <triqs__gfs__gf_meshLTimfreqGT__evaluate>`                               |  |
+-----------------------------------------------------------------------------------------+--+
| :ref:`hdf5_scheme <triqs__gfs__gf_meshLTimfreqGT__hdf5_scheme>`                         |  |
+-----------------------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    gf_meshLTimfreqGT/constructor
    gf_meshLTimfreqGT/set_tail_fit_parameters
    gf_meshLTimfreqGT/get_tail_fitter
    gf_meshLTimfreqGT/operator==
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
    gf_meshLTimfreqGT/is_within_boundary
    gf_meshLTimfreqGT/get_interpolation_data
    gf_meshLTimfreqGT/evaluate
    gf_meshLTimfreqGT/hdf5_scheme


Non Member functions
--------------------

+-----------------------------------------------------------+--+
| :ref:`h5_write <triqs__gfs__gf_meshLTimfreqGT__h5_write>` |  |
+-----------------------------------------------------------+--+
| :ref:`h5_read <triqs__gfs__gf_meshLTimfreqGT__h5_read>`   |  |
+-----------------------------------------------------------+--+
| :ref:`boost <triqs__gfs__gf_meshLTimfreqGT__boost>`       |  |
+-----------------------------------------------------------+--+

.. toctree::
    :hidden:

    gf_meshLTimfreqGT/h5_write
    gf_meshLTimfreqGT/h5_read
    gf_meshLTimfreqGT/boost