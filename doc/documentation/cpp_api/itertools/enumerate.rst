..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _itertools__enumerate:

itertools::enumerate
====================

*#include <itertools.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename R>`
       | auto :red:`enumerate` (R && :param:`range`)





Lazy-enumerate a range (similar to Python enumerate)

The function returns a iterable lazy object. When iterated upon,
this object yields a pair (n,x) where :
  * n is the index (starting at 0)
  * x is in the object in the range





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`R` Type of the ranges


Parameters
^^^^^^^^^^

 * :param:`range` The range to enumerate


Example
^^^^^^^

..
   Included automatically from /mnt/home/wentzell/Dropbox/Coding/triqs/doc/documentation/examples/itertools/enumerate.cpp

.. code-block:: cpp

    #include <itertools/itertools.hpp>
    #include <vector>
    #include <iostream>

    int main() {

      std::vector<int> v {10,11,12,13,14};

      for (auto [n,x] : itertools::enumerate(v))
        std::cout << n << "  " << x << std::endl;
    }


*Output*

.. code-block:: text

    0  10
    1  11
    2  12
    3  13
    4  14