..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _det_manip_inverse_matrix:

triqs::det_manip::det_manip::inverse_matrix
===========================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Returns M^{-1}(i,j) `
       | det_manip::value_type :red:`inverse_matrix` (int :param:`i`, int :param:`j`) const

    2. | :cppbrief:`Returns the inverse matrix. Warning : this is slow, since it create a new copy, and reorder the lines/cols`
       | det_manip::matrix_type :red:`inverse_matrix` () const

Documentation



 **1)**          warning : need to invert the 2 permutations: (AP)^-1= P^-1 A^-1.