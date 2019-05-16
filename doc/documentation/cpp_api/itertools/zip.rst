..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _itertools__zip:

itertools::zip
==============

*#include <itertools.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename R>`
       | auto :red:`zip` (R &&... :param:`ranges`)





Generate a zip of the ranges (similar to Python zip).

The function returns a iterable lazy object. When iterated upon,
this object yields a tuple of the objects in the ranges.





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`R` Type of the ranges


Parameters
^^^^^^^^^^

 * :param:`ranges`
     The ranges to zip.

     .. warning::
          The ranges have to be equal lengths or behaviour is undefined.


Example
^^^^^^^

..
   Included automatically from /mnt/home/wentzell/Dropbox/Coding/triqs/doc/documentation/examples/itertools/zip.cpp

.. code-block:: cpp

    #include <itertools/itertools.hpp>
    #include <vector>
    #include <iostream>

    int main() {

      std::vector<int> v1 {10,11,12,13,14};
      std::vector<int> v2 {0,1,2,3,4};

      for (auto [x,y] : itertools::zip(v1, v2))
        std::cout << x << "  " << y << std::endl;
    }


*Output*

.. code-block:: text

    10  0
    11  1
    12  2
    13  3
    14  4