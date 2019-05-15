..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _transform:

transform
=========


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Transform (lazy)applies a unary lambda function to every`
       | :green:`template<typename T, typename L>`
       | auto :red:`transform` (T && :param:`range`, L :param:`lambda`)





   element of a range. It returns itself a range.





Parameters
^^^^^^^^^^

 * **range**: The range that the lambda is applied to