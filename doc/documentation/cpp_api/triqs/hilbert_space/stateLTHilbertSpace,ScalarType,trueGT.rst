..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__hilbert_space__stateLTHilbertSpace_ScalarType_trueGT:

triqs::hilbert_space::state<HilbertSpace,ScalarType,true>
=========================================================

*#include <triqs/hilbert_space/state.hpp>*

.. rst-class:: cppsynopsis

     template<typename HilbertSpace, typename ScalarType> class  :red:`state<HilbertSpace,ScalarType,true>`

State: implementation based on a map

 This specialization can work well on huge Hilbert spaces, as long as there are not
 too many non-vanishing amplitudes  in the state.


Template parameters
-------------------

 * **HilbertSpace**: Hilbert space type, one of [[hilbert_space]] and [[sub_hilbert_space]]

 * **ScalarType**: Amplitude type, normally `double` or `std::complex<double>`


Member types
------------

+-----------------+--------------+------------------------------------------------+
| value_type      | ScalarType   |  Accessor to `ScalarType` template parameter   |
+-----------------+--------------+------------------------------------------------+
| hilbert_space_t | HilbertSpace |  Accessor to `HilbertSpace` template parameter |
+-----------------+--------------+------------------------------------------------+


Member functions
----------------

+-------------------------------------------------------------------------------------------------+--+
| :ref:`(constructor) <triqs__hilbert_space__stateLTHilbertSpace_ScalarType_trueGT__constructor>` |  |
+-------------------------------------------------------------------------------------------------+--+
| :ref:`size <triqs__hilbert_space__stateLTHilbertSpace_ScalarType_trueGT__size>`                 |  |
+-------------------------------------------------------------------------------------------------+--+
| :ref:`operator() <triqs__hilbert_space__stateLTHilbertSpace_ScalarType_trueGT__operator()>`     |  |
+-------------------------------------------------------------------------------------------------+--+
| :ref:`operator+= <triqs__hilbert_space__stateLTHilbertSpace_ScalarType_trueGT__operator+=>`     |  |
+-------------------------------------------------------------------------------------------------+--+
| :ref:`operator-= <triqs__hilbert_space__stateLTHilbertSpace_ScalarType_trueGT__operator-=>`     |  |
+-------------------------------------------------------------------------------------------------+--+
| :ref:`operator*= <triqs__hilbert_space__stateLTHilbertSpace_ScalarType_trueGT__operator*=>`     |  |
+-------------------------------------------------------------------------------------------------+--+
| :ref:`operator/= <triqs__hilbert_space__stateLTHilbertSpace_ScalarType_trueGT__operator/=>`     |  |
+-------------------------------------------------------------------------------------------------+--+
| :ref:`get_hilbert <triqs__hilbert_space__stateLTHilbertSpace_ScalarType_trueGT__get_hilbert>`   |  |
+-------------------------------------------------------------------------------------------------+--+
| :ref:`set_hilbert <triqs__hilbert_space__stateLTHilbertSpace_ScalarType_trueGT__set_hilbert>`   |  |
+-------------------------------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    stateLTHilbertSpace,ScalarType,trueGT/constructor
    stateLTHilbertSpace,ScalarType,trueGT/size
    stateLTHilbertSpace,ScalarType,trueGT/operator()
    stateLTHilbertSpace,ScalarType,trueGT/operator+=
    stateLTHilbertSpace,ScalarType,trueGT/operator-=
    stateLTHilbertSpace,ScalarType,trueGT/operator*=
    stateLTHilbertSpace,ScalarType,trueGT/operator/=
    stateLTHilbertSpace,ScalarType,trueGT/get_hilbert
    stateLTHilbertSpace,ScalarType,trueGT/set_hilbert


Non Member functions
--------------------

+-----------------------------------------------------------------------------------------------+--+
| :ref:`dot_product <triqs__hilbert_space__stateLTHilbertSpace_ScalarType_trueGT__dot_product>` |  |
+-----------------------------------------------------------------------------------------------+--+
| :ref:`foreach <triqs__hilbert_space__stateLTHilbertSpace_ScalarType_trueGT__foreach>`         |  |
+-----------------------------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    stateLTHilbertSpace,ScalarType,trueGT/dot_product
    stateLTHilbertSpace,ScalarType,trueGT/foreach