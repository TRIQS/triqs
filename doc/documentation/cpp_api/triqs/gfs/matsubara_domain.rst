..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__gfs__matsubara_domain:

triqs::gfs::matsubara_domain
============================

Defined in header <**>

.. code-block:: c

    template<bool IsFreq> class matsubara_domain

---------------------------------------------------------------------------------------------------------

     The domain
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

+---------------------------------------------------+--+
| :ref:`constructor <matsubara_domain_constructor>` |  |
+---------------------------------------------------+--+
| :ref:`operator== <matsubara_domain_operator==>`   |  |
+---------------------------------------------------+--+
| :ref:`operator!= <matsubara_domain_operator!=>`   |  |
+---------------------------------------------------+--+
| :ref:`serialize <matsubara_domain_serialize>`     |  |
+---------------------------------------------------+--+

.. toctree::
    :hidden:

    matsubara_domain/constructor
    matsubara_domain/operator==
    matsubara_domain/operator!=
    matsubara_domain/serialize


Non Member functions
--------------------

+-----------------------------------------------------+-----------------+
| :ref:`h5_write <matsubara_domain_h5_write>`         | Write into HDF5 |
+-----------------------------------------------------+-----------------+
| :ref:`h5_read <matsubara_domain_h5_read>`           | Read from HDF5  |
+-----------------------------------------------------+-----------------+
| :ref:`boost <matsubara_domain_boost>`               |                 |
+-----------------------------------------------------+-----------------+
| :ref:`operator\<\< <matsubara_domain_operatorLTLT>` |                 |
+-----------------------------------------------------+-----------------+

.. toctree::
    :hidden:

    matsubara_domain/h5_write
    matsubara_domain/h5_read
    matsubara_domain/boost
    matsubara_domain/operator<<