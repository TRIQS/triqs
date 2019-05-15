..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__gfs__matsubara_freq:

triqs::gfs::matsubara_freq
==========================

Defined in header <**>

.. code-block:: c

     class matsubara_freq

A matsubara frequency, i.e.

   * n : int, the index
   * beta : double, the temperature inverse
   * statistic : Fermion or Boson

 * Can be casted into a complex.

 * Every operations is done by casting to complex, except addition and substraction of matsubara_freq, which return matsubara_freq
   and work on the index
       * n : int, the index
   * beta : double, the temperature inverse
   * statistic : Fermion or Boson

 * Can be casted into a complex.

 * Every operations is done by casting to complex, except addition and substraction of matsubara_freq, which return matsubara_freq
   and work on the index

Public members
--------------

+-----------+----------------------------+--+
| n         | long                       |  |
+-----------+----------------------------+--+
| beta      | double                     |  |
+-----------+----------------------------+--+
| statistic | triqs::gfs::statistic_enum |  |
+-----------+----------------------------+--+


Member types
------------

+--------+----------------------+--+
| cast_t | std::complex<double> |  |
+--------+----------------------+--+


Member functions
----------------

+-------------------------------------------------+--+
| :ref:`constructor <matsubara_freq_constructor>` |  |
+-------------------------------------------------+--+

.. toctree::
    :hidden:

    matsubara_freq/constructor