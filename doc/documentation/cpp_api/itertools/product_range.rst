..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _product_range:

product_range
=============


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`A product of an arbitrary number of integer ranges`
       | :green:`template<typename Integers, typename EnableIf = std::enable_if_t<(std::is_integral_v<Integers>and...),int>`
       | auto :red:`product_range` (Integers... :param:`Is`)





   given a set of integers or an integer tuple





Template parameters
^^^^^^^^^^^^^^^^^^^

 * **Integers**: The integer types