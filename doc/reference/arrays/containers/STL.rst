.. highlight:: c

.. _STL:

Iterators and STL
##################################################################

Standard iterators are provided that model the boost Mutable_ForwardIterator and ForwardIterator concepts
(and hence are STL compliant).

The iterator implements also two additional methods :

* it can be casted to a bool: it is true iif the iteration is not at end.
* it.indices(): returns const & to the indices at the point of the iteration.

Examples:: 

 array<long,2> A (2,3);
 for (auto it = A.begin(); it; ++it) *it =it.indices()[0] + 10 *it.indices()[1];


Some examples of usage: 


.. triqs_example:: ./STL_0.cpp
