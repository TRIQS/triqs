..
   Generated automatically by cpp2rst

.. highlight:: c


.. _det_manip_roll_matrix:

roll_matrix
===========

**Synopsis**:

.. code-block:: c

    int roll_matrix (triqs::det_manip::det_manip::RollDirection roll)

"Cyclic Rolling" of the determinant.


Documentation
-------------


 Right : Move the Nth col to the first col cyclically.
 Left  : Move the first col to the Nth, cyclically.
 Up    : Move the first row to the Nth, cyclically.
 Down  : Move the Nth row to the first row cyclically.

 Returns -1 is the roll changes the sign of the det, 1 otherwise
 NB : this routine is not a try_xxx : it DOES make the modification and does not need to be completed...
 WHY is it like this ???? : try_roll : return det +1/-1.