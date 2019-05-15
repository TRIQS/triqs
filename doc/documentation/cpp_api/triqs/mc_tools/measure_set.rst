..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__mc_tools__measure_set:

triqs::mc_tools::measure_set
============================

Defined in header <**>

.. code-block:: c

    template<typename MCSignType> class measure_set

--------------------------------------------------------------------




Member types
------------

+---------------+----------------------------------+--+
| measure_ptr_t | typename m_map_t::const_iterator |  |
+---------------+----------------------------------+--+


Member functions
----------------

+------------------------------------------------------+--------------------------------------------------+
| :ref:`constructor <measure_set_constructor>`         |                                                  |
+------------------------------------------------------+--------------------------------------------------+
| :ref:`operator= <measure_set_operator=>`             |                                                  |
+------------------------------------------------------+--------------------------------------------------+
| :ref:`insert <measure_set_insert>`                   | Register the Measure M with a name               |
+------------------------------------------------------+--------------------------------------------------+
| :ref:`remove <measure_set_remove>`                   | Remove the measure m.                            |
+------------------------------------------------------+--------------------------------------------------+
| :ref:`has <measure_set_has>`                         | Does qmc have a measure named name               |
+------------------------------------------------------+--------------------------------------------------+
| :ref:`accumulate <measure_set_accumulate>`           |                                                  |
+------------------------------------------------------+--------------------------------------------------+
| :ref:`names <measure_set_names>`                     |                                                  |
+------------------------------------------------------+--------------------------------------------------+
| :ref:`get_timings <measure_set_get_timings>`         | Pretty print the timings of all measures         |
+------------------------------------------------------+--------------------------------------------------+
| :ref:`collect_results <measure_set_collect_results>` | gather result for all measure, on communicator c |
+------------------------------------------------------+--------------------------------------------------+

.. toctree::
    :hidden:

    measure_set/constructor
    measure_set/operator=
    measure_set/insert
    measure_set/remove
    measure_set/has
    measure_set/accumulate
    measure_set/names
    measure_set/get_timings
    measure_set/collect_results


Non Member functions
--------------------

+----------------------------------------+----------------+
| :ref:`h5_write <measure_set_h5_write>` | HDF5 interface |
+----------------------------------------+----------------+
| :ref:`h5_read <measure_set_h5_read>`   |                |
+----------------------------------------+----------------+

.. toctree::
    :hidden:

    measure_set/h5_write
    measure_set/h5_read