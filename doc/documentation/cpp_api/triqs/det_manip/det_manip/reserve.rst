..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__reserve:

triqs::det_manip::det_manip::reserve
====================================

*#include <triqs/det_manip/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | void :red:`reserve` (size_t :param:`new_size`)





Like for std::vector, reserve memory for a bigger size.
Preserves only the matrix, not the temporary working vectors/matrices, so do NOT use it
between a try_XXX and a complete_operation





Parameters
^^^^^^^^^^

 * :param:`new_size` The new size of the reserved memory