..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _range_constructor:

itertools::range::constructor
=============================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Default constructor`
       | :red:`range` ()

    2. | :cppbrief:`Constructor`
       | :red:`range` (range::index_t :param:`first__`,
       |   range::index_t :param:`last__`,
       |   range::index_t :param:`step__` = 1)

    3. | :cppbrief:`Constructor`
       | :red:`range` (range::index_t :param:`i`)

Documentation



 **1)**
 range() stands for the whole set of indices in the dimension (like `:` in python)
 Example: A(range(), 0) takes the first column of A



 **3)**
 Constructs range(0, i, 1)





Parameters
^^^^^^^^^^

 * **first__**: The first index of the range

 * **last__**: The first index past the end of the range

 * **step__**: The step between two indices