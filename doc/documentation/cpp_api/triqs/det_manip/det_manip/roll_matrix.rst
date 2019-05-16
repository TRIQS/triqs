..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__roll_matrix:

triqs::det_manip::det_manip::roll_matrix
========================================

*#include <triqs/det_manip/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | int :red:`roll_matrix` (det_manip::RollDirection :param:`roll`)





"Cyclic Rolling" of the determinant.

Right : Move the Nth col to the first col cyclically.
Left  : Move the first col to the Nth, cyclically.
Up    : Move the first row to the Nth, cyclically.
Down  : Move the Nth row to the first row cyclically.

Returns -1 is the roll changes the sign of the det, 1 otherwise
NB : this routine is not a try_xxx : it DOES make the modification and does not need to be completed...
WHY is it like this ???? : try_roll : return det +1/-1.