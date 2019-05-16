..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__flatten_2d:

triqs::gfs::flatten_2d
======================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename T, int R>`
       | array<T, 2> :red:`flatten_2d` (array_const_view<T, R> :param:`a`, int :param:`n`)





Makes a copy of the array in matrix, whose first dimension is the n-th dimension of a
and the second dimension are the flattening of the other dimensions, in the original order





Parameters
^^^^^^^^^^

 * :param:`a` : array

 * :param:`n` : the dimension to preserve.


Returns
^^^^^^^

: a matrix, copy of the data