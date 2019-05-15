..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _mc_generic_warmup:

triqs::mc_tools::mc_generic::warmup
===================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | int :red:`warmup` (uint64_t :param:`n_warmup_cycles`, int64_t :param:`length_cycle`, std::function<bool ()> :param:`stop_callback`)

    2. | :cppbrief:`Warmup the Monte-Carlo configuration`
       | int :red:`warmup` (uint64_t :param:`n_warmup_cycles`,
       |   int64_t :param:`length_cycle`,
       |   std::function<bool ()> :param:`stop_callback`,
       |   MCSignType :param:`sign_init`)

Documentation





Parameters
^^^^^^^^^^

 * **n_warmup_cycles**: Number of QMC cycles in the warmup

 * **length_cycle**: Number of QMC move attempts in one cycle

 * **stop_callback**: A callback function () -> bool. It is called after each cycle
                                to and the computation stops when it returns true.
                                Typically used to set up the time limit, cf doc.

 * **sign_init**: The sign of the initial configuration's weight [optional]


Returns
^^^^^^^

 * =  =============================================
    0  if the computation has run until the end
    1  if it has been stopped by stop_callback
    2  if it has been stopped by receiving a signal
    =  =============================================