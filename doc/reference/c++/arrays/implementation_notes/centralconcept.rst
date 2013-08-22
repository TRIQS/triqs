The central concept : HasImmutableArrayInterface
=======================================================

The central concept used by the library (both in the colloquial and in the technical acceptance of the word)
is the notion of **Immutable Array Interface**.

This is just a formal definition of what has an object has to implement ("which concept it has to model")
to look like an array.

Basically, the answer is simple, an array is a (discrete) map between a domain of indices into an algebra
(typically R, ou C). Therefore it has to defined : 

* a domain of indices, which can be enumerated, e.g. cartesian products of integer ranges.
* an evaluation method [] so that a[ I ] has a `value_type` type for any index I in the domain.

The precise concept if defined at :ref:`HasImmutableArrayInterface` concept.

Examples : 

* a matrix has a domain of range(0,n1) x range(0,n2).
* a general array has a domain of dimension rank and values in R, C, other arrays...
* a triangular matrix has a more complex domain...
* any algebraic expression of arrays, matrices are like this (and those one are certainly NOT mutable objects !).

* What is then a (partial) view/slice of an array ?

  Simply another map on the same data.

* Do you want to see a 4 indices arrays as a matrix of double indices ?

  Simply change the map...

* Why is this concept useful ?

  Because if you program another object of your own, which models it, 
  it is guaranteed to interact properly with arrays....



