..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _itertools__range__constructor:

itertools::range::constructor
=============================

*#include <itertools/range.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :red:`range` ()

    2. | :red:`range` (range::index_t :param:`first__`,
       |      range::index_t :param:`last__`,
       |      range::index_t :param:`step__` = 1)

    3. | :red:`range` (range::index_t :param:`i`)

Documentation



 **1)**   Default constructor

range() stands for the whole set of indices in the dimension (like `:` in python)
Example: A(range(), 0) takes the first column of A



 **2)**   Constructor



 **3)**   Constructor

Constructs range(0, i, 1)





Parameters
^^^^^^^^^^

 * :param:`first__` The first index of the range

 * :param:`last__` The first index past the end of the range

 * :param:`step__` The step between two indices