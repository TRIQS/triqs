..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _flatten_2d:

flatten_2d
==========


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Makes a copy of the array in matrix, whose first dimension is the n-th dimension of a`
       | :green:`template<typename T, int R>`
       | array<T, 2> :red:`flatten_2d` (array_const_view<T, R> :param:`a`, int :param:`n`)





   and the second dimension are the flattening of the other dimensions, in the original order





Parameters
^^^^^^^^^^

 * **a**: : array

 * **n**: : the dimension to preserve.


Returns
^^^^^^^

 * : a matrix, copy of the data