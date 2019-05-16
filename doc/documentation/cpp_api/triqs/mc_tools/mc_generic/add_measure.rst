..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__mc_tools__mc_generic__add_measure:

triqs::mc_tools::mc_generic::add_measure
========================================

*#include <triqs/mc_tools/mc_generic.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename MeasureType>`
       | typename measure_set<MCSignType>::measure_ptr_t :red:`add_measure` (MeasureType && :param:`m`,
       |              std::string :param:`name`,
       |              bool :param:`enable_timer` = true)





Register a measure

If the measure m is an rvalue, it is moved into the mc_generic, otherwise is copied into it.





Parameters
^^^^^^^^^^

 * :param:`M`                        The measure. Must model Measure concept

 * :param:`name`                     Name of the measure