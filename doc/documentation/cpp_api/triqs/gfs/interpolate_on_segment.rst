..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__interpolate_on_segment:

triqs::gfs::interpolate_on_segment
==================================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | interpol_data_lin_t<long, 2> :red:`interpolate_on_segment` (double :param:`x`, double :param:`x_min`, double :param:`delta_x`, long :param:`imax`)





Fit the two closest points for x on [x_min, x_max], with a linear weight w





Parameters
^^^^^^^^^^

 * :param:`x` : the point

 * :param:`i_max` : maximum index

 * :param:`x_min` : the window starts. It ends at x_min + i_max* delta_x

 * :param:`delta_x` Throws if x is not in the window