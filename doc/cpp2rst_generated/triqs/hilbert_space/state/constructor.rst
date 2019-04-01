..
   Generated automatically by cpp2rst

.. highlight:: c


.. _state_constructor:

constructor
===========

**Synopsis**:

.. code-block:: c

    void state<type-parameter-0-0, type-parameter-0-1, false> ()                            (1)

    void state<type-parameter-0-0, type-parameter-0-1, false> (const HilbertSpace & hs)     (2)

(1)Construct a new state object


Documentation
-------------


   The constructed state is dummy state not belonging to any Hilbert space. **It should not be used in expressions!**

(2)Construct a new state object


Parameters
----------

 * **hs**: Hilbert space the new state belongs to


Documentation
-------------