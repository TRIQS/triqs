..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _itertools__slice:

itertools::slice
================

*#include <itertools.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename T>`
       | auto :red:`slice` (T && :param:`range`, std::ptrdiff_t :param:`start_idx`, std::ptrdiff_t :param:`end_idx`)





Lazy-slice a range.
This function returns itself a slice of the initial range





Parameters
^^^^^^^^^^

 * :param:`range` The range to slice

 * :param:`start_idx` The index to start the slice at

 * :param:`end_idx` The index one past the end of the sliced range