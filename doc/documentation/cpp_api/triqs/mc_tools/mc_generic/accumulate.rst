..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _mc_generic_accumulate:

triqs::mc_tools::mc_generic::accumulate
=======================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Accumulate/Measure`
       | int :red:`accumulate` (uint64_t :param:`n_accumulation_cycles`,
       |   int64_t :param:`length_cycle`,
       |   std::function<bool ()> :param:`stop_callback`)







Parameters
^^^^^^^^^^

 * **n_accumulation_cycles**: Number of QMC cycles in the accumulation (measures are done after each cycle).

 * **length_cycle**: Number of QMC move attempts in one cycle

 * **stop_callback**: A callback function () -> bool. It is called after each cycle
                                to and the computation stops when it returns true.
                                Typically used to set up the time limit, cf doc.


Returns
^^^^^^^

 * =  =============================================
    0  if the computation has run until the end
    1  if it has been stopped by stop_callback
    2  if it has been stopped by receiving a signal
    =  =============================================