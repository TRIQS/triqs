..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _det_manip_try_change_col:

triqs::det_manip::det_manip::try_change_col
===========================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Consider the change the column j and the corresponding y.`
       | det_manip::value_type :red:`try_change_col` (size_t :param:`j`, det_manip::y_type const & :param:`y`)






 Returns the ratio of det Minv_new / det Minv.
 This routine does NOT make any modification. It has to be completed with complete_operation().