..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__get_singular_threshold:

triqs::det_manip::det_manip::get_singular_threshold
===================================================

*#include <triqs/det_manip/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | double :red:`get_singular_threshold` () const





Get the number below which abs(det) is considered 0. If <0, the test will be isnormal(abs(det))