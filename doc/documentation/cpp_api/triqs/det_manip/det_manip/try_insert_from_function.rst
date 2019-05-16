..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__try_insert_from_function:

triqs::det_manip::det_manip::try_insert_from_function
=====================================================

*#include <triqs/det_manip/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename Fx, typename Fy>`
       | det_manip::value_type :red:`try_insert_from_function` (size_t :param:`i`,
       |                           size_t :param:`j`,
       |                           Fx :param:`fx`,
       |                           Fy :param:`fy`,
       |                           const det_manip::value_type :param:`ksi`)





fx gives the new line coefficients, fy gives the new column coefficients and ksi is the last coeff (at the intersection of the line and the column).