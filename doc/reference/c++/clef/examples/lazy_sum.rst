.. highlight:: c

A lazy sum
--------------
 
Problem : we want

* a functional `sum` that sums a function f over various domains, with various methods.

* that this functionnal accepts lazy expressions as arguments.

This is done by the little code: 

.. literalinclude:: src/sum_functional.cpp

which will print :
  
.. literalinclude:: src/sum_functional2.output


