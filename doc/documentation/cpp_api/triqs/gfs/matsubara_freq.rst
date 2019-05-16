..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__matsubara_freq:

triqs::gfs::matsubara_freq
==========================

*#include <triqs/gfs.hpp>*

.. rst-class:: cppsynopsis

      class  :red:`matsubara_freq`

A matsubara frequency, i.e.
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

+----------------------------------------------------------------+--+
| :ref:`(constructor) <triqs__gfs__matsubara_freq__constructor>` |  |
+----------------------------------------------------------------+--+

.. toctree::
    :hidden:

    matsubara_freq/constructor