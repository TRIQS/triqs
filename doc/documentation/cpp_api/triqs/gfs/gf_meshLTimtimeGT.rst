..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__gfs__gf_meshLTimtimeGT:

triqs::gfs::gf_mesh<imtime>
===========================

Defined in header <**>

.. code-block:: c

     class gf_mesh<imtime>

Imaginary-time mesh


  Mesh for the imaginary-time axis from :math:`0` to :math:`\beta`.
  The time points are defined as follows, for :math:`n=0{\dots}N-1`:

.. math::
		\tau_n=\frac{n}{N-1}\beta

..
 where :math:`N` is the size of the mesh.

  Mesh for the imaginary-time axis from :math:`0` to :math:`\beta`.
  The time points are defined as follows, for :math:`n=0{\dots}N-1`:

.. math::
		\tau_n=\frac{n}{N-1}\beta

..
 where :math:`N` is the size of the mesh.

.. figure:: ../../../triqs/gfs/matsubara_imtime.png
   :alt:  Pictorial representation of ``gf_mesh<imtime>{beta, Fermion/Boson, 4}``.
   :align: center

Pictorial representation of ``gf_mesh<imtime>{beta, Fermion/Boson, 4}``.


Member types
------------

+-------+------------------------------------+--+
| B     | linear_mesh<matsubara_time_domain> |  |
+-------+------------------------------------+--+
| var_t | imtime                             |  |
+-------+------------------------------------+--+


Member functions
----------------

+------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| :ref:`constructor <gf_meshLTimtimeGT_constructor>`               |                                                                                                                         |
+------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| :ref:`is_within_boundary <gf_meshLTimtimeGT_is_within_boundary>` | For imtime the point is always in the mesh, since we use anti-periodicity or periodicity. Needed for cartesian product. |
+------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| :ref:`evaluate <gf_meshLTimtimeGT_evaluate>`                     | evaluation                                                                                                              |
+------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| :ref:`hdf5_scheme <gf_meshLTimtimeGT_hdf5_scheme>`               | -------------------- hdf5 -------------------                                                                           |
+------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------------+

.. toctree::
    :hidden:

    gf_meshLTimtimeGT/constructor
    gf_meshLTimtimeGT/is_within_boundary
    gf_meshLTimtimeGT/evaluate
    gf_meshLTimtimeGT/hdf5_scheme


Non Member functions
--------------------

+------------------------------------------------------+------------------------------------------------+
| :ref:`operator\<\< <gf_meshLTimtimeGT_operatorLTLT>` | -------------------- print ------------------- |
+------------------------------------------------------+------------------------------------------------+
| :ref:`h5_write <gf_meshLTimtimeGT_h5_write>`         |                                                |
+------------------------------------------------------+------------------------------------------------+
| :ref:`h5_read <gf_meshLTimtimeGT_h5_read>`           |                                                |
+------------------------------------------------------+------------------------------------------------+

.. toctree::
    :hidden:

    gf_meshLTimtimeGT/operator<<
    gf_meshLTimtimeGT/h5_write
    gf_meshLTimtimeGT/h5_read