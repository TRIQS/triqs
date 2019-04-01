..
   Generated automatically by cpp2rst

.. highlight:: c


.. _state:

state
=======

State: implementation based on `triqs::arrays::vector`



**Synopsis**:

.. code-block:: c

    template<typename HilbertSpace, typename ScalarType> class state;


Member types
------------

+-----------------+---------------------------------------------------+
| Member type     | Comment                                           |
+=================+===================================================+
| value_type      | /// Accessor to `ScalarType` template parameter   |
+-----------------+---------------------------------------------------+
| hilbert_space_t | /// Accessor to `HilbertSpace` template parameter |
+-----------------+---------------------------------------------------+


Member functions
----------------

+----------------------------------------------------------+------------------------------------------------------------------+
| Member function                                          | Comment                                                          |
+==========================================================+==================================================================+
| :ref:`size <state_size>`                                 | Return the dimension of the associated Hilbert space             |
+----------------------------------------------------------+------------------------------------------------------------------+
| :ref:`operator() <state_operator()>`                     | Access to individual amplitudes                                  |
+----------------------------------------------------------+------------------------------------------------------------------+
| :ref:`operator+= -= \*= /= <state_operator+= -= \*= /=>` | In-place addition of another state                               |
+----------------------------------------------------------+------------------------------------------------------------------+
| :ref:`amplitudes <state_amplitudes>`                     | Direct access to the storage container (`triqs::arrays::vector`) |
+----------------------------------------------------------+------------------------------------------------------------------+
| :ref:`get_hilbert <state_get_hilbert>`                   | Return a constant reference to the associated Hilbert space      |
+----------------------------------------------------------+------------------------------------------------------------------+
| :ref:`set_hilbert <state_set_hilbert>`                   | Reset the associated Hilbert space                               |
+----------------------------------------------------------+------------------------------------------------------------------+
| :ref:`constructor <state_constructor>`                   | Construct a new state object                                     |
+----------------------------------------------------------+------------------------------------------------------------------+

.. toctree::
    :hidden:

    state/size
    state/operator()
    state/operator+= -= \*= /=
    state/amplitudes
    state/get_hilbert
    state/set_hilbert
    state/constructor


Non Member functions
--------------------

+----------------------------------------+------------------------------------------------------+
| Non member function                    | Comment                                              |
+========================================+======================================================+
| :ref:`dot_product <state_dot_product>` | Calculate scalar product of two states               |
+----------------------------------------+------------------------------------------------------+
| :ref:`foreach <state_foreach>`         | Apply a callable object to all amplitudes of a state |
+----------------------------------------+------------------------------------------------------+

.. toctree::
    :hidden:

    state/dot_product
    state/foreach