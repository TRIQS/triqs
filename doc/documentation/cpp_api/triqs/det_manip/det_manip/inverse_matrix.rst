..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__inverse_matrix:

triqs::det_manip::det_manip::inverse_matrix
===========================================

*#include <triqs/det_manip/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | det_manip::value_type :red:`inverse_matrix` (int :param:`i`, int :param:`j`) const

    2. | det_manip::matrix_type :red:`inverse_matrix` () const

Documentation



 **1)**   Returns M^{-1}(i,j)
     // warning : need to invert the 2 permutations: (AP)^-1= P^-1 A^-1.



 **2)**   Returns the inverse matrix. Warning : this is slow, since it create a new copy, and reorder the lines/cols