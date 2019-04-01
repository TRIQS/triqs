..
   Generated automatically by cpp2rst

.. highlight:: c


.. _mc_generic_add_measure:

add_measure
===========

**Synopsis**:

.. code-block:: c

    template<typename MeasureType>
    typename measure_set<MCSignType>::measure_ptr_t add_measure (MeasureType && m,
   std::string name, bool enable_timer = true)

Register a measure


Parameters
----------

 * **M**:                        The measure. Must model Measure concept

 * **name**:                     Name of the measure


Documentation
-------------


 If the measure m is an rvalue, it is moved into the mc_generic, otherwise is copied into it.