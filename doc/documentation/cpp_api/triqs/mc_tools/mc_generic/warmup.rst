..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__mc_tools__mc_generic__warmup:

triqs::mc_tools::mc_generic::warmup
===================================

*#include <triqs/mc_tools/mc_generic.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | int :red:`warmup` (uint64_t :param:`n_warmup_cycles`,
       |         int64_t :param:`length_cycle`,
       |         std::function<bool ()> :param:`stop_callback`,
       |         MCSignType :param:`sign_init`)





Warmup the Monte-Carlo configuration





Parameters
^^^^^^^^^^

 * :param:`n_warmup_cycles`         Number of QMC cycles in the warmup

 * :param:`length_cycle`            Number of QMC move attempts in one cycle

 * :param:`stop_callback`           A callback function () -> bool. It is called after each cycle
                                to and the computation stops when it returns true.
                                Typically used to set up the time limit, cf doc.

 * :param:`sign_init`               The sign of the initial configuration's weight [optional]


Returns
^^^^^^^

    =  =============================================
    0  if the computation has run until the end
    1  if it has been stopped by stop_callback
    2  if it has been stopped by receiving a signal
    =  =============================================