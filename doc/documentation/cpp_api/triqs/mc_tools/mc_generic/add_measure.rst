..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _mc_generic_add_measure:

triqs::mc_tools::mc_generic::add_measure
========================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Register a measure`
       | :green:`template<typename MeasureType>`
       | typename measure_set<MCSignType>::measure_ptr_t :red:`add_measure` (MeasureType && :param:`m`,
       |   std::string :param:`name`,
       |   bool :param:`enable_timer` = true)






 If the measure m is an rvalue, it is moved into the mc_generic, otherwise is copied into it.





Parameters
^^^^^^^^^^

 * **M**: The measure. Must model Measure concept

 * **name**: Name of the measure