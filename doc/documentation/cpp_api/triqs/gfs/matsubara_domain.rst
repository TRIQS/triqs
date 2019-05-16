..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__matsubara_domain:

triqs::gfs::matsubara_domain
============================

*#include <triqs/gfs.hpp>*

.. rst-class:: cppsynopsis

     template<bool IsFreq> class  :red:`matsubara_domain`

---------------------------------------------------------------------------------------------------------
 The domain


Public members
--------------

+-----------+----------------------------+--+
| beta      | double                     |  |
+-----------+----------------------------+--+
| statistic | triqs::gfs::statistic_enum |  |
+-----------+----------------------------+--+


Member types
------------

+---------+-----------------------------------------------------------------------+--+
| point_t | typename std::conditional<IsFreq, std::complex<double>, double>::type |  |
+---------+-----------------------------------------------------------------------+--+


Member functions
----------------

+------------------------------------------------------------------+--+
| :ref:`(constructor) <triqs__gfs__matsubara_domain__constructor>` |  |
+------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    matsubara_domain/constructor


Non Member functions
--------------------

+----------------------------------------------------------+--+
| :ref:`h5_write <triqs__gfs__matsubara_domain__h5_write>` |  |
+----------------------------------------------------------+--+
| :ref:`h5_read <triqs__gfs__matsubara_domain__h5_read>`   |  |
+----------------------------------------------------------+--+
| :ref:`boost <triqs__gfs__matsubara_domain__boost>`       |  |
+----------------------------------------------------------+--+

.. toctree::
    :hidden:

    matsubara_domain/h5_write
    matsubara_domain/h5_read
    matsubara_domain/boost