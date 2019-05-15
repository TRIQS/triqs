..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _stride:

stride
======


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Lazy-stride a range.`
       | :green:`template<typename T>`
       | auto :red:`stride` (T && :param:`range`, std::ptrdiff_t :param:`stride`)





   This function returns itself a subrange of the initial range
 by considering only every N-th element





Parameters
^^^^^^^^^^

 * **range**: The range to take the subrange of

 * **stride**: The numer of elements to skip