triqs::mc_tools
###############



Type aliases
------------

+--------------------------------+------------------------------------------------------+-------------------------------------------------------------------------+
| h5_rw_lambda_t                 | std::function<void (h5::group, const std::string &)> |  ----------------- h5 detection -----------------------                 |
+--------------------------------+------------------------------------------------------+-------------------------------------------------------------------------+
| collect_statistics_rw_lambda_t | std::function<void (mpi::communicator)>              |  ----------------- collect_statistics detection ----------------------- |
+--------------------------------+------------------------------------------------------+-------------------------------------------------------------------------+


Classes
-------

.. table::
   :width: 50% 50%

+-------------------------------------------------------------+--+
| :ref:`is_callable <triqs__mc_tools__is_callable>`           |  |
+-------------------------------------------------------------+--+
| :ref:`measure_aux <triqs__mc_tools__measure_aux>`           |  |
+-------------------------------------------------------------+--+
| :ref:`has_attempt <triqs__mc_tools__has_attempt>`           |  |
+-------------------------------------------------------------+--+
| :ref:`make_h5_impl <triqs__mc_tools__make_h5_impl>`         |  |
+-------------------------------------------------------------+--+
| :ref:`measure <triqs__mc_tools__measure>`                   |  |
+-------------------------------------------------------------+--+
| :ref:`measure_set <triqs__mc_tools__measure_set>`           |  |
+-------------------------------------------------------------+--+
| :ref:`random_generator <triqs__mc_tools__random_generator>` |  |
+-------------------------------------------------------------+--+
| :ref:`move_set <triqs__mc_tools__move_set>`                 |  |
+-------------------------------------------------------------+--+
| :ref:`move <triqs__mc_tools__move>`                         |  |
+-------------------------------------------------------------+--+
| :ref:`mc_generic <triqs__mc_tools__mc_generic>`             |  |
+-------------------------------------------------------------+--+

.. toctree::
    :hidden:

    mc_tools/is_callable
    mc_tools/measure_aux
    mc_tools/has_attempt
    mc_tools/make_h5_impl
    mc_tools/measure
    mc_tools/measure_set
    mc_tools/random_generator
    mc_tools/move_set
    mc_tools/move
    mc_tools/mc_generic


Functions
---------

+-------------------------------------------------------------------------+--+
| :ref:`random_generator_names <triqs__mc_tools__random_generator_names>` |  |
+-------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    mc_tools/random_generator_names