.. highlight:: c

Operations: array and matrix/vector algebras 
=======================================================

Arithmetic operations
-----------------------------

Arrays and matrices can be combined in formal algebraic expressions, which models the :ref:`ImmutableCuboidArray` concept.
This algebraic expressions can therefore be used in assignment array/matrix contructors.
For example: 

.. triqs_example:: ./algebras_0.cpp
Arrays vs matrices
----------------------

Because their multiplication is not the same, arrays and matrices algebras cannot be mixed.
Mixing them in expression would therefore be meaningless and it is therefore not allowed.
However, you can always use e.g. `matrix_view` from a array of rank 2 :
  
.. triqs_example:: ./algebras_1.cpp
.. note::

   Making such a view is very cheap, it only copies the index systems. Nevertheless
   this can still cause significant overhead in very intense loops by disturbing
   optimizers.

   
Performance
---------------------------------------------

The performance of such compact writing is as good as "hand-written" code or even better.

Indeed, the operations are implemented with the `expression templates` technique.
The principle is that the result of A+B is **NOT** an array, but a more complex type which stores
the expression using the naturally recursive structure of templates.

Expressions models :ref:`ImmutableCuboidArray` concept.
They behave like an immutable array: they have a domain, they can be evaluated.
Hence they can used *anywhere* an object modeling this concept is accepted, e.g.: 

* array, matrix contruction
* operator =, +=, -=, ...

When an array in assigned (or constructed from) such expression, it fills itself
by evaluating the expression.

This technique allows the elimination of temporaries, so that the clear and readable code::

   Z= A + 2*B + C/2;

is in fact rewritten by the compiler into ::
 
   for (i,j,...) indices of A, B : 
     C(i,j) = A(i,j) + 2* B(i,j) + C(i,j)/2

instead of making a chain of temporaries (C/2, 2*B, 2*B + C/2...) that "ordinary" object-oriented programming would produce.
As a result, the produced code is as fast as if you were writing the loop yourself,
but with several advantages: 

* It is more **compact** and **readable**: you don't have to write the loop, and the indices range are computed automatically.
* It is much better for **optimization**: 
  
  * What you want is to tell the compiler/library to compute this expression, not *how* to do it optimally on a given machine.
  * For example, since the traversal order of indices is decided at compile time, the library can traverse the data
    in an optimal way, allowing machine-dependent optimization.
  * The library can perform easy optimisations behind the scene when possible, e.g. for vector it can use blas.
 
Expressions are lazy....
---------------------------
Note that expressions are lazy objects. It does nothing when constructed, it just "record" the mathematical expression ::

   auto e =  A + 2*B;             // expression, purely formal, no computation is done
   cout<< e <<endl ;              // prints the expression
   cout<< e(1,2) <<endl ;         // evaluates just at a point
   cout<< e.domain() <<endl ;     // just computes its domain
   array<long,2> D(e);            // now really makes the computation and store the result in D.
   D = 2*A +B;                    // reassign D to the evaluation of the expression.

