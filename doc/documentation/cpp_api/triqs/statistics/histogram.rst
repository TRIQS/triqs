..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__statistics__histogram:

triqs::statistics::histogram
============================

*#include <triqs/statistics/histograms.hpp>*

.. rst-class:: cppsynopsis

      class  :red:`histogram`

Statistical histogram

  This class serves to sample a continuous random variable, and to 'bin' it.
  It divides a given range of real values into a series of equal intervals,
  and counts amounts of samples falling into each interval.
  The histogram keeps track of the total number of the sampled values, as well
  as of the lost samples that lie outside the chosen range.


Member functions
----------------

+------------------------------------------------------------------+--+
| :ref:`(constructor) <triqs__statistics__histogram__constructor>` |  |
+------------------------------------------------------------------+--+
| :ref:`operator\<\< <triqs__statistics__histogram__operatorLTLT>` |  |
+------------------------------------------------------------------+--+
| :ref:`mesh_point <triqs__statistics__histogram__mesh_point>`     |  |
+------------------------------------------------------------------+--+
| :ref:`size <triqs__statistics__histogram__size>`                 |  |
+------------------------------------------------------------------+--+
| :ref:`limits <triqs__statistics__histogram__limits>`             |  |
+------------------------------------------------------------------+--+
| :ref:`data <triqs__statistics__histogram__data>`                 |  |
+------------------------------------------------------------------+--+
| :ref:`n_data_pts <triqs__statistics__histogram__n_data_pts>`     |  |
+------------------------------------------------------------------+--+
| :ref:`n_lost_pts <triqs__statistics__histogram__n_lost_pts>`     |  |
+------------------------------------------------------------------+--+
| :ref:`clear <triqs__statistics__histogram__clear>`               |  |
+------------------------------------------------------------------+--+
| :ref:`hdf5_scheme <triqs__statistics__histogram__hdf5_scheme>`   |  |
+------------------------------------------------------------------+--+
| :ref:`serialize <triqs__statistics__histogram__serialize>`       |  |
+------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    histogram/constructor
    histogram/operator<<
    histogram/mesh_point
    histogram/size
    histogram/limits
    histogram/data
    histogram/n_data_pts
    histogram/n_lost_pts
    histogram/clear
    histogram/hdf5_scheme
    histogram/serialize


Non Member functions
--------------------

+--------------------------------------------------------------------+--+
| :ref:`operator+ <triqs__statistics__histogram__operator+>`         |  |
+--------------------------------------------------------------------+--+
| :ref:`mpi_broadcast <triqs__statistics__histogram__mpi_broadcast>` |  |
+--------------------------------------------------------------------+--+
| :ref:`mpi_reduce <triqs__statistics__histogram__mpi_reduce>`       |  |
+--------------------------------------------------------------------+--+
| :ref:`h5_write <triqs__statistics__histogram__h5_write>`           |  |
+--------------------------------------------------------------------+--+
| :ref:`h5_read <triqs__statistics__histogram__h5_read>`             |  |
+--------------------------------------------------------------------+--+
| :ref:`operator\<\< <triqs__statistics__histogram__operatorLTLT>`   |  |
+--------------------------------------------------------------------+--+
| :ref:`boost <triqs__statistics__histogram__boost>`                 |  |
+--------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    histogram/operator+
    histogram/mpi_broadcast
    histogram/mpi_reduce
    histogram/h5_write
    histogram/h5_read
    histogram/operator<<
    histogram/boost