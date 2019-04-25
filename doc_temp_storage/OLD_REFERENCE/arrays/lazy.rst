.. highlight:: c

.. _Lazy:

Interaction with CLEF expressions 
============================================

* The containers and their view classes can be used with the :doc:`../clef/contents` library: 

  * They can be called with CLEF expressions.
  * :doc:`Automatic assignment<../clef/assign>` has been set up.

* Using the CLEF library offers a quick and efficient way to fill an array with multiple advantages: 

  * It is simpler and more readeable than a series of for loops.
  * It is usually more optimal since the for loops are automatically written in the TraversalOrder of the array.


* **Example**: 

.. triqs_example:: ./lazy_0.cpp
.. note:: 
   The syntax uses a <<, not = since the array is not assigned to an expression
   but filled by the evaluation thereof.



  

