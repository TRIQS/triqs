.. highlight:: c
 
FAQ
======

How do I iterate on my array ?
-----------------------------------


There are different way to iterate on the element of an array, recommended in this order from the most simple/efficient to the most complex/slow : 

* To assign data into an array, the simplest and efficient way is to use 
  automatic assignment with lazy expressions, Cf :ref:`Lazy`.

* For a more general case, where one does not simply assign a value to the array element, 
  use a *foreach* construct, Cf :ref:`Foreach`.

* You can use STL algorithms, since arrays have STL compliant iterators.
  The performance may be lower than *foreach* loops (never better anyhow).

* Of course, one can still use a simple for loop, but this is not recommended in general
  since it is more error prone and less optimal.
  

