..
   Generated automatically by cpp2rst

.. highlight:: c


.. _mc_generic_warmup:

warmup
======

**Synopsis**:

.. code-block:: c

    int warmup (uint64_t n_warmup_cycles, int64_t length_cycle, std::function<bool ()>
   stop_callback)                           (1)

    int warmup (uint64_t n_warmup_cycles, int64_t length_cycle, std::function<bool ()>
   stop_callback, MCSignType sign_init)     (2)

(1)


(2)Warmup the Monte-Carlo configuration


Parameters
----------

 * **n_warmup_cycles**:         Number of QMC cycles in the warmup

 * **length_cycle**:            Number of QMC move attempts in one cycle

 * **stop_callback**:           A callback function () -> bool. It is called after each cycle
                                to and the computation stops when it returns true.
                                Typically used to set up the time limit, cf doc.

 * **sign_init**:               The sign of the initial configuration's weight [optional]




Documentation
-------------