..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__hilbert_space__stateLTHilbertSpace_ScalarType_trueGT:

triqs::hilbert_space::state<HilbertSpace,ScalarType,true>
=========================================================

Defined in header <*triqs/hilbert_space/state.hpp*>

.. code-block:: c

    template<typename HilbertSpace, typename ScalarType> class state<HilbertSpace,ScalarType,true>

State: implementation based on a map


  This specialization can work well on huge Hilbert spaces, as long as there are not
  too many non-vanishing amplitudes  in the state.

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

+------------------------------------------------------------------------+-------------------------------------------------------------+
| :ref:`constructor <stateLTHilbertSpace_ScalarType_trueGT_constructor>` | Construct a new state object                                |
+------------------------------------------------------------------------+-------------------------------------------------------------+
| :ref:`size <stateLTHilbertSpace_ScalarType_trueGT_size>`               | Return the dimension of the associated Hilbert space        |
+------------------------------------------------------------------------+-------------------------------------------------------------+
| :ref:`operator() <stateLTHilbertSpace_ScalarType_trueGT_operator()>`   | Access to individual amplitudes                             |
+------------------------------------------------------------------------+-------------------------------------------------------------+
| :ref:`operator+= <stateLTHilbertSpace_ScalarType_trueGT_operator+=>`   | In-place addition of another state                          |
+------------------------------------------------------------------------+-------------------------------------------------------------+
| :ref:`operator-= <stateLTHilbertSpace_ScalarType_trueGT_operator-=>`   | In-place subtraction of another state                       |
+------------------------------------------------------------------------+-------------------------------------------------------------+
| :ref:`operator*= <stateLTHilbertSpace_ScalarType_trueGT_operator*=>`   | In-place multiplication by a scalar                         |
+------------------------------------------------------------------------+-------------------------------------------------------------+
| :ref:`operator/= <stateLTHilbertSpace_ScalarType_trueGT_operator/=>`   | In-place division by a scalar                               |
+------------------------------------------------------------------------+-------------------------------------------------------------+
| :ref:`get_hilbert <stateLTHilbertSpace_ScalarType_trueGT_get_hilbert>` | Return a constant reference to the associated Hilbert space |
+------------------------------------------------------------------------+-------------------------------------------------------------+
| :ref:`set_hilbert <stateLTHilbertSpace_ScalarType_trueGT_set_hilbert>` | Reset the associated Hilbert space                          |
+------------------------------------------------------------------------+-------------------------------------------------------------+

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

+------------------------------------------------------------------------+--------------------------------------------------------------------+
| :ref:`dot_product <stateLTHilbertSpace_ScalarType_trueGT_dot_product>` | Calculate scalar product of two states                             |
+------------------------------------------------------------------------+--------------------------------------------------------------------+
| :ref:`foreach <stateLTHilbertSpace_ScalarType_trueGT_foreach>`         | Apply a callable object to **non-vanishing** amplitudes of a state |
+------------------------------------------------------------------------+--------------------------------------------------------------------+

.. toctree::
    :hidden:

    stateLTHilbertSpace,ScalarType,trueGT/dot_product
    stateLTHilbertSpace,ScalarType,trueGT/foreach