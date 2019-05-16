..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf_meshLTimtimeGT:

triqs::gfs::gf_mesh<imtime>
===========================

*#include <triqs/gfs.hpp>*

.. rst-class:: cppsynopsis

      class  :red:`gf_mesh<imtime>`

Imaginary-time mesh

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

+-------------------------------------------------------------------------------+--+
| :ref:`(constructor) <triqs__gfs__gf_meshLTimtimeGT__constructor>`             |  |
+-------------------------------------------------------------------------------+--+
| :ref:`is_within_boundary <triqs__gfs__gf_meshLTimtimeGT__is_within_boundary>` |  |
+-------------------------------------------------------------------------------+--+
| :ref:`evaluate <triqs__gfs__gf_meshLTimtimeGT__evaluate>`                     |  |
+-------------------------------------------------------------------------------+--+
| :ref:`hdf5_scheme <triqs__gfs__gf_meshLTimtimeGT__hdf5_scheme>`               |  |
+-------------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    gf_meshLTimtimeGT/constructor
    gf_meshLTimtimeGT/is_within_boundary
    gf_meshLTimtimeGT/evaluate
    gf_meshLTimtimeGT/hdf5_scheme


Non Member functions
--------------------

+-------------------------------------------------------------------+--+
| :ref:`operator\<\< <triqs__gfs__gf_meshLTimtimeGT__operatorLTLT>` |  |
+-------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    gf_meshLTimtimeGT/operator<<