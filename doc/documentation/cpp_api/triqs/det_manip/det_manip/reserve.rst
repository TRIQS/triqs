..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _det_manip_reserve:

triqs::det_manip::det_manip::reserve
====================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Like for std::vector, reserve memory for a bigger size.`
       | void :red:`reserve` (size_t :param:`new_size`)





   Preserves only the matrix, not the temporary working vectors/matrices, so do NOT use it
 between a try_XXX and a complete_operation





Parameters
^^^^^^^^^^

 * **new_size**: The new size of the reserved memory