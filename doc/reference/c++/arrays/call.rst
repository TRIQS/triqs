.. highlight:: c

Operator() : summary
==================================

* To summarize the previous items, here is the list of possible call of array, matrix, vector and their view.

  If A is an array, and Args ... the arguments, the return type of the  call ::

    A( Args ... args) 
 
  is summarized in the following table.

===========================================   ========================================================
Arguments                                     Returns
===========================================   ========================================================
None                                          a complete view of the array
One at least Args is clef expression          a clef expression
Args is a set of Range, Ellipsis, size_t      a view class representing the partial view
===========================================   ========================================================


* Example ::

    // write here an example with several variant
