..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _det_manip_try_change_row:

triqs::det_manip::det_manip::try_change_row
===========================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Consider the change the row i and the corresponding x.`
       | det_manip::value_type :red:`try_change_row` (size_t :param:`i`, det_manip::x_type const & :param:`x`)






 Returns the ratio of det Minv_new / det Minv.
 This routine does NOT make any modification. It has to be completed with complete_operation().