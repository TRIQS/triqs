..
   Generated automatically by cpp2rst

.. highlight:: c


.. _mc_generic_accumulate:

accumulate
==========

**Synopsis**:

.. code-block:: c

    int accumulate (uint64_t n_accumulation_cycles, int64_t length_cycle,
   std::function<bool ()> stop_callback)

Accumulate/Measure


Parameters
----------

 * **n_accumulation_cycles**:   Number of QMC cycles in the accumulation (measures are done after each cycle).

 * **length_cycle**:            Number of QMC move attempts in one cycle

 * **stop_callback**:           A callback function () -> bool. It is called after each cycle
                                to and the computation stops when it returns true.
                                Typically used to set up the time limit, cf doc.




Documentation
-------------