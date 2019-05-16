..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _itertools__stride:

itertools::stride
=================

*#include <itertools.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename T>`
       | auto :red:`stride` (T && :param:`range`, std::ptrdiff_t :param:`stride`)





Lazy-stride a range.
This function returns itself a subrange of the initial range
by considering only every N-th element





Parameters
^^^^^^^^^^

 * :param:`range` The range to take the subrange of

 * :param:`stride` The numer of elements to skip