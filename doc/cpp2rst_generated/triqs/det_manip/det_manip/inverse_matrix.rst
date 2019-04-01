..
   Generated automatically by cpp2rst

.. highlight:: c


.. _det_manip_inverse_matrix:

inverse_matrix
==============

**Synopsis**:

.. code-block:: c

    triqs::det_manip::det_manip::value_type inverse_matrix (int i, int j)     (1)

    triqs::det_manip::det_manip::matrix_type inverse_matrix ()                (2)

(1)Returns M^{-1}(i,j)


(2)Returns the inverse matrix. Warning : this is slow, since it create a new copy, and reorder the lines/cols