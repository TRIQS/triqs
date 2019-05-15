..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _interpolate_on_segment:

interpolate_on_segment
======================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Fit the two closest points for x on [x_min, x_max], with a linear weight w`
       | interpol_data_lin_t<long, 2> :red:`interpolate_on_segment` (double :param:`x`, double :param:`x_min`, double :param:`delta_x`, long :param:`imax`)







Parameters
^^^^^^^^^^

 * **x**: : the point

 * **i_max**: : maximum index

 * **x_min**: : the window starts. It ends at x_min + i_max* delta_x

 * **delta_x**: Throws if x is not in the window