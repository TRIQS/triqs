..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _itertools__product:

itertools::product
==================

*#include <itertools.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename T>`
       | auto :red:`product` (T &&... :param:`ranges`)





Lazy-product of multiple ranges. This function returns itself a range of tuple<T...>.
Iterating over it will yield all combinations of the different range values.
Note: The ranges are incremented beginning with the leftmost range.





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`T` The types of the different ranges


Parameters
^^^^^^^^^^

 * :param:`ranges` The ranges to zip. Note: They have to be of equal length!