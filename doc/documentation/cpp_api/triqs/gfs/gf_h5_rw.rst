..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__gfs__gf_h5_rw:

triqs::gfs::gf_h5_rw
====================

Defined in header <**>

.. code-block:: c

    template<typename V, typename T> class gf_h5_rw

----------------------------------------------------------

  HDF5
  Traits to read/write in hdf5 files.
  Can be specialized for some case (Cf block). Defined below
--------------------------------------------------------
      HDF5
  Traits to read/write in hdf5 files.
  Can be specialized for some case (Cf block). Defined below
--------------------------------------------------------

Member functions
----------------

+-------------------------------+----------------------------------------------------------------------------------------------------------------------------+
| :ref:`write <gf_h5_rw_write>` | template <typename G> static void write(h5::group gr, G const &g) { _write(gr, gf_h5_before_write<V, T>::invoke(gr, g)); } |
+-------------------------------+----------------------------------------------------------------------------------------------------------------------------+
| :ref:`read <gf_h5_rw_read>`   |                                                                                                                            |
+-------------------------------+----------------------------------------------------------------------------------------------------------------------------+

.. toctree::
    :hidden:

    gf_h5_rw/write
    gf_h5_rw/read