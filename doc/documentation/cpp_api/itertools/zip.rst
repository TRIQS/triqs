..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _zip:

zip
===


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Lazy-zip a range.`
       | :green:`template<typename T>`
       | auto :red:`zip` (T &&... :param:`ranges`)





   This function returns itself a range of tuple<T...>





Parameters
^^^^^^^^^^

 * **ranges**: The ranges to zip. Note: They have to be of equal length!