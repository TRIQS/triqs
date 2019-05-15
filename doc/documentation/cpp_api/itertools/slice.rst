..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _slice:

slice
=====


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Lazy-slice a range.`
       | :green:`template<typename T>`
       | auto :red:`slice` (T && :param:`range`, std::ptrdiff_t :param:`start_idx`, std::ptrdiff_t :param:`end_idx`)





   This function returns itself a slice of the initial range





Parameters
^^^^^^^^^^

 * **range**: The range to slice

 * **start_idx**: The index to start the slice at

 * **end_idx**: The index one past the end of the sliced range