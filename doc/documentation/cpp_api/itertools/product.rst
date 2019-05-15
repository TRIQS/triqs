..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _product:

product
=======


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Lazy-product of multiple ranges. This function returns itself a range of tuple<T...>.`
       | :green:`template<typename T>`
       | auto :red:`product` (T &&... :param:`ranges`)





   Iterating over it will yield all combinations of the different range values.
 Note: The ranges are incremented beginning with the leftmost range.





Template parameters
^^^^^^^^^^^^^^^^^^^

 * **T**: The types of the different ranges


Parameters
^^^^^^^^^^

 * **ranges**: The ranges to zip. Note: They have to be of equal length!