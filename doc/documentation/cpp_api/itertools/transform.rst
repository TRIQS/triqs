..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _itertools__transform:

itertools::transform
====================

*#include <itertools.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename T, typename L>`
       | auto :red:`transform` (T && :param:`range`, L :param:`lambda`)





Transform (lazy)applies a unary lambda function to every
element of a range. It returns itself a range.





Parameters
^^^^^^^^^^

 * :param:`range` The range that the lambda is applied to