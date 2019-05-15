..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__statistics__histogram:

triqs::statistics::histogram
============================

Defined in header <*triqs/statistics/histograms.hpp*>

.. code-block:: c

     class histogram

Statistical histogram


   This class serves to sample a continuous random variable, and to 'bin' it.
   It divides a given range of real values into a series of equal intervals,
   and counts amounts of samples falling into each interval.
   The histogram keeps track of the total number of the sampled values, as well
   as of the lost samples that lie outside the chosen range.

   This class serves to sample a continuous random variable, and to 'bin' it.
   It divides a given range of real values into a series of equal intervals,
   and counts amounts of samples falling into each interval.
   The histogram keeps track of the total number of the sampled values, as well
   as of the lost samples that lie outside the chosen range.

Member functions
----------------

+----------------------------------------------+--------------------------------------+
| :ref:`constructor <histogram_constructor>`   | Constructor                          |
+----------------------------------------------+--------------------------------------+
| :ref:`operator\<\< <histogram_operatorLTLT>` | Bins a real value into the histogram |
+----------------------------------------------+--------------------------------------+
| :ref:`mesh_point <histogram_mesh_point>`     | Get position of bin's center         |
+----------------------------------------------+--------------------------------------+
| :ref:`size <histogram_size>`                 | Get number of histogram bins         |
+----------------------------------------------+--------------------------------------+
| :ref:`limits <histogram_limits>`             | Return boundaries of the histogram   |
+----------------------------------------------+--------------------------------------+
| :ref:`data <histogram_data>`                 | Read-only access to the data storage |
+----------------------------------------------+--------------------------------------+
| :ref:`n_data_pts <histogram_n_data_pts>`     | Get number of accumulated samples    |
+----------------------------------------------+--------------------------------------+
| :ref:`n_lost_pts <histogram_n_lost_pts>`     | Get number of discarded samples      |
+----------------------------------------------+--------------------------------------+
| :ref:`clear <histogram_clear>`               | Reset all histogram values to 0      |
+----------------------------------------------+--------------------------------------+
| :ref:`hdf5_scheme <histogram_hdf5_scheme>`   | Get HDF5 scheme name                 |
+----------------------------------------------+--------------------------------------+
| :ref:`serialize <histogram_serialize>`       | Serialization                        |
+----------------------------------------------+--------------------------------------+

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

+------------------------------------------------+---------------------------------------------------------------------------------+
| :ref:`pdf <histogram_pdf>`                     | Normalise histogram to get probability density function (PDF)                   |
+------------------------------------------------+---------------------------------------------------------------------------------+
| :ref:`cdf <histogram_cdf>`                     | Integrate and normalise histogram to get cumulative distribution function (CDF) |
+------------------------------------------------+---------------------------------------------------------------------------------+
| :ref:`operator+ <histogram_operator+>`         | Addition of histograms                                                          |
+------------------------------------------------+---------------------------------------------------------------------------------+
| :ref:`mpi_broadcast <histogram_mpi_broadcast>` | MPI-broadcast histogram                                                         |
+------------------------------------------------+---------------------------------------------------------------------------------+
| :ref:`mpi_reduce <histogram_mpi_reduce>`       | MPI-reduce histogram                                                            |
+------------------------------------------------+---------------------------------------------------------------------------------+
| :ref:`h5_write <histogram_h5_write>`           | Write histogram to HDF5                                                         |
+------------------------------------------------+---------------------------------------------------------------------------------+
| :ref:`h5_read <histogram_h5_read>`             | Read histogram from HDF5                                                        |
+------------------------------------------------+---------------------------------------------------------------------------------+
| :ref:`operator\<\< <histogram_operatorLTLT>`   | Output stream insertion                                                         |
+------------------------------------------------+---------------------------------------------------------------------------------+
| :ref:`boost <histogram_boost>`                 |                                                                                 |
+------------------------------------------------+---------------------------------------------------------------------------------+

.. toctree::
    :hidden:

    histogram/pdf
    histogram/cdf
    histogram/operator+
    histogram/mpi_broadcast
    histogram/mpi_reduce
    histogram/h5_write
    histogram/h5_read
    histogram/operator<<
    histogram/boost