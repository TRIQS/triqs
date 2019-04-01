..
   Generated automatically by cpp2rst

.. highlight:: c


.. code-block:: c

    #include <triqs/mc_tools.hpp>

.. _mc_generic:

mc_generic
============

Generic Monte Carlo class.


 TBR


**Synopsis**:

.. code-block:: c

    template<typename MCSignType> class mc_generic;

 TBR


Member functions
----------------

+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| Member function                                                               | Comment                                                                            |
+===============================================================================+====================================================================================+
| :ref:`add_move <mc_generic_add_move>`                                         | Register a move                                                                    |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`add_measure <mc_generic_add_measure>`                                   | Register a measure                                                                 |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`add_measure_aux <mc_generic_add_measure_aux>`                           | Register a common part for several measure [EXPERIMENTAL: API WILL CHANGE]         |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`rm_measure <mc_generic_rm_measure>`                                     | Deregister a measure                                                               |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`set_after_cycle_duty <mc_generic_set_after_cycle_duty>`                 | Sets a function called after each cycle                                            |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`warmup <mc_generic_warmup>`                                             |                                                                                    |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`accumulate <mc_generic_accumulate>`                                     | Accumulate/Measure                                                                 |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`warmup_and_accumulate <mc_generic_warmup_and_accumulate>`               |                                                                                    |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`collect_results <mc_generic_collect_results>`                           | Reduce the results of the measures, and reports some statistics                    |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`get_acceptance_rates <mc_generic_get_acceptance_rates>`                 | The acceptance rates of all move                                                   |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`get_percent <mc_generic_get_percent>`                                   | The current percents done                                                          |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`get_rng <mc_generic_get_rng>`                                           | An access to the random number generator                                           |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`rng <mc_generic_rng>`                                                   |                                                                                    |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`get_current_cycle_number <mc_generic_get_current_cycle_number>`         | The current cycle number                                                           |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`get_config_id <mc_generic_get_config_id>`                               | The current number of the visited configuration. Updated after each accept/reject. |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`get_duration <mc_generic_get_duration>`                                 | The duration of the last run in seconds                                            |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`get_total_time <mc_generic_get_total_time>`                             | The total time of the last run in seconds                                          |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`get_warmup_time <mc_generic_get_warmup_time>`                           | The time spent on warmup in seconds                                                |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`get_warmup_time_HHMMSS <mc_generic_get_warmup_time_HHMMSS>`             | The time spent on warmup in hours, minutes, and seconds                            |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`get_accumulation_time <mc_generic_get_accumulation_time>`               | The time spent on accumulation in seconds                                          |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`get_accumulation_time_HHMMSS <mc_generic_get_accumulation_time_HHMMSS>` | The time spent on warmup in hours, minutes, and seconds                            |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+
| :ref:`constructor <mc_generic_constructor>`                                   | Constructor                                                                        |
+-------------------------------------------------------------------------------+------------------------------------------------------------------------------------+

.. toctree::
    :hidden:

    mc_generic/add_move
    mc_generic/add_measure
    mc_generic/add_measure_aux
    mc_generic/rm_measure
    mc_generic/set_after_cycle_duty
    mc_generic/warmup
    mc_generic/accumulate
    mc_generic/warmup_and_accumulate
    mc_generic/collect_results
    mc_generic/get_acceptance_rates
    mc_generic/get_percent
    mc_generic/get_rng
    mc_generic/rng
    mc_generic/get_current_cycle_number
    mc_generic/get_config_id
    mc_generic/get_duration
    mc_generic/get_total_time
    mc_generic/get_warmup_time
    mc_generic/get_warmup_time_HHMMSS
    mc_generic/get_accumulation_time
    mc_generic/get_accumulation_time_HHMMSS
    mc_generic/constructor


Non Member functions
--------------------

+---------------------------------------+----------------+
| Non member function                   | Comment        |
+=======================================+================+
| :ref:`h5_write <mc_generic_h5_write>` | HDF5 interface |
+---------------------------------------+----------------+
| :ref:`h5_read <mc_generic_h5_read>`   | HDF5 interface |
+---------------------------------------+----------------+

.. toctree::
    :hidden:

    mc_generic/h5_write
    mc_generic/h5_read