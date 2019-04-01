..
   Generated automatically by cpp2rst

.. highlight:: c


.. _imperative_operator_update_coeffs:

update_coeffs
=============

**Synopsis**:

.. code-block:: c

    template<typename Lambda>
    void update_coeffs (Lambda L)

Apply a callable object to each coefficient of the operator by reference


Template parameters
-------------------

 * **Lambda**: Type of the callable object



Parameters
----------

 * **L**: Callable object


Documentation
-------------


   The callable object must take one argument convertible to `ScalarType &`