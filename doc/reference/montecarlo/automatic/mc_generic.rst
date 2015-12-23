Full reference
*******************

..
   Generated automatically using the command :
   c++2doc.py all_triqs.hpp

.. highlight:: c

.. _mc_generic:

.. code-block:: c

    #include <triqs/mc_tools.hpp>

mc_generic
============

**Synopsis**:

.. code-block:: c

     template<typename MCSignType> class mc_generic;

Generic Monte Carlo class.

 TBR




Member functions
-----------------

+-----------------------------------------------------------------------+-----------------------------------------------------------------------------------------+
| Member function                                                       | Comment                                                                                 |
+=======================================================================+=========================================================================================+
| :ref:`constructor <mc_generic_constructor>`                           | Constructor                                                                             |
+-----------------------------------------------------------------------+-----------------------------------------------------------------------------------------+
| :ref:`add_move <mc_generic_add_move>`                                 | Register a move                                                                         |
+-----------------------------------------------------------------------+-----------------------------------------------------------------------------------------+
| :ref:`add_measure <mc_generic_add_measure>`                           | Register a measure                                                                      |
+-----------------------------------------------------------------------+-----------------------------------------------------------------------------------------+
| :ref:`add_measure_aux <mc_generic_add_measure_aux>`                   | Register a common part for several measure [EXPERIMENTAL: API WILL CHANGE]              |
+-----------------------------------------------------------------------+-----------------------------------------------------------------------------------------+
| :ref:`set_after_cycle_duty <mc_generic_set_after_cycle_duty>`         | Sets a function called after each cycle                                                 |
+-----------------------------------------------------------------------+-----------------------------------------------------------------------------------------+
| :ref:`start <mc_generic_start>`                                       | Starts the Monte Carlo                                                                  |
+-----------------------------------------------------------------------+-----------------------------------------------------------------------------------------+
| :ref:`collect_results <mc_generic_collect_results>`                   | Reduce the results of the measures, and reports some statistics                         |
+-----------------------------------------------------------------------+-----------------------------------------------------------------------------------------+
| :ref:`get_acceptance_rates <mc_generic_get_acceptance_rates>`         | The acceptance rates of all move                                                        |
+-----------------------------------------------------------------------+-----------------------------------------------------------------------------------------+
| :ref:`get_percent <mc_generic_get_percent>`                           | The current percents done                                                               |
+-----------------------------------------------------------------------+-----------------------------------------------------------------------------------------+
| :ref:`get_rng <mc_generic_get_rng>`                                   | An access to the random number generator                                                |
+-----------------------------------------------------------------------+-----------------------------------------------------------------------------------------+
| :ref:`get_current_cycle_number <mc_generic_get_current_cycle_number>` | The current cycle number                                                                |
+-----------------------------------------------------------------------+-----------------------------------------------------------------------------------------+
| :ref:`get_config_id <mc_generic_get_config_id>`                       | The current number of the visited configuration. Updated after each accept/reject.      |
+-----------------------------------------------------------------------+-----------------------------------------------------------------------------------------+
| :ref:`is_thermalized <mc_generic_is_thermalized>`                     | Is the qmc thermalized, i.e. has it run more than n_warmup_cycles given at construction |
+-----------------------------------------------------------------------+-----------------------------------------------------------------------------------------+

.. toctree::

  :hidden:

   mc_generic/constructor
   mc_generic/add_move
   mc_generic/add_measure
   mc_generic/add_measure_aux
   mc_generic/set_after_cycle_duty
   mc_generic/start
   mc_generic/collect_results
   mc_generic/get_acceptance_rates
   mc_generic/get_percent
   mc_generic/get_rng
   mc_generic/get_current_cycle_number
   mc_generic/get_config_id
   mc_generic/is_thermalized

Non Member functions
-----------------------

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
