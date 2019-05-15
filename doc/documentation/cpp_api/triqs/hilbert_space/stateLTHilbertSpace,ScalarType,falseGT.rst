..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__hilbert_space__stateLTHilbertSpace_ScalarType_falseGT:

triqs::hilbert_space::state<HilbertSpace,ScalarType,false>
==========================================================

Defined in header <**>

.. code-block:: c

    template<typename HilbertSpace, typename ScalarType> class state<HilbertSpace,ScalarType,false>

State: implementation based on `triqs::arrays::vector`




Member types
------------

+-----------------+--------------+------------------------------------------------+
| value_type      | ScalarType   |  Accessor to `ScalarType` template parameter   |
+-----------------+--------------+------------------------------------------------+
| hilbert_space_t | HilbertSpace |  Accessor to `HilbertSpace` template parameter |
+-----------------+--------------+------------------------------------------------+


Member functions
----------------

+-------------------------------------------------------------------------+------------------------------------------------------------------+
| :ref:`constructor <stateLTHilbertSpace_ScalarType_falseGT_constructor>` | Construct a new state object                                     |
+-------------------------------------------------------------------------+------------------------------------------------------------------+
| :ref:`size <stateLTHilbertSpace_ScalarType_falseGT_size>`               | Return the dimension of the associated Hilbert space             |
+-------------------------------------------------------------------------+------------------------------------------------------------------+
| :ref:`operator() <stateLTHilbertSpace_ScalarType_falseGT_operator()>`   | Access to individual amplitudes                                  |
+-------------------------------------------------------------------------+------------------------------------------------------------------+
| :ref:`operator+= <stateLTHilbertSpace_ScalarType_falseGT_operator+=>`   | In-place addition of another state                               |
+-------------------------------------------------------------------------+------------------------------------------------------------------+
| :ref:`operator-= <stateLTHilbertSpace_ScalarType_falseGT_operator-=>`   | In-place subtraction of another state                            |
+-------------------------------------------------------------------------+------------------------------------------------------------------+
| :ref:`operator*= <stateLTHilbertSpace_ScalarType_falseGT_operator*=>`   | In-place multiplication by a scalar                              |
+-------------------------------------------------------------------------+------------------------------------------------------------------+
| :ref:`operator/= <stateLTHilbertSpace_ScalarType_falseGT_operator/=>`   | In-place division by a scalar                                    |
+-------------------------------------------------------------------------+------------------------------------------------------------------+
| :ref:`amplitudes <stateLTHilbertSpace_ScalarType_falseGT_amplitudes>`   | Direct access to the storage container (`triqs::arrays::vector`) |
+-------------------------------------------------------------------------+------------------------------------------------------------------+
| :ref:`get_hilbert <stateLTHilbertSpace_ScalarType_falseGT_get_hilbert>` | Return a constant reference to the associated Hilbert space      |
+-------------------------------------------------------------------------+------------------------------------------------------------------+
| :ref:`set_hilbert <stateLTHilbertSpace_ScalarType_falseGT_set_hilbert>` | Reset the associated Hilbert space                               |
+-------------------------------------------------------------------------+------------------------------------------------------------------+

.. toctree::
    :hidden:

    stateLTHilbertSpace,ScalarType,falseGT/constructor
    stateLTHilbertSpace,ScalarType,falseGT/size
    stateLTHilbertSpace,ScalarType,falseGT/operator()
    stateLTHilbertSpace,ScalarType,falseGT/operator+=
    stateLTHilbertSpace,ScalarType,falseGT/operator-=
    stateLTHilbertSpace,ScalarType,falseGT/operator*=
    stateLTHilbertSpace,ScalarType,falseGT/operator/=
    stateLTHilbertSpace,ScalarType,falseGT/amplitudes
    stateLTHilbertSpace,ScalarType,falseGT/get_hilbert
    stateLTHilbertSpace,ScalarType,falseGT/set_hilbert


Non Member functions
--------------------

+-------------------------------------------------------------------------+------------------------------------------------------+
| :ref:`dot_product <stateLTHilbertSpace_ScalarType_falseGT_dot_product>` | Calculate scalar product of two states               |
+-------------------------------------------------------------------------+------------------------------------------------------+
| :ref:`foreach <stateLTHilbertSpace_ScalarType_falseGT_foreach>`         | Apply a callable object to all amplitudes of a state |
+-------------------------------------------------------------------------+------------------------------------------------------+

.. toctree::
    :hidden:

    stateLTHilbertSpace,ScalarType,falseGT/dot_product
    stateLTHilbertSpace,ScalarType,falseGT/foreach