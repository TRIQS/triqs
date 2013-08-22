.. highlight:: c

Operations : array and matrix/vector algebras 
=======================================================

Operations
------------

Arrays and matrices can be combined in formal algebraic expressions, which models the :ref:`ImmutableCuboidArray` concept.
This algebraic expressions can therefore be used as RHS of assignment (SEE) or in array/matrix contructors.
For example ; 

.. compileblock::

   #include <triqs/arrays.hpp>
   using triqs::arrays::array; 
   int main() {  
    array<long,2> A (2,2), B(2,2), C;
    C= A + 2*B;
    array<long,2> D = A+ 2*B;
    array<double,2> F = 0.5 * A;  // Type promotion is automatic
   }

Arrays vs matrices
----------------------

Because their multiplication is not the same, arrays and matrices algebras can not be mixed.
Mixing them in expression would therefore be meaningless and it is therefore not allowed ::

   array<long,2> A;
   matrix<long,2> M;
   M + A; // --> ERROR. Rejected by the compiler.

However, you can always make a matrix_view from a array of rank 2 ::
  
   A + make_matrix_view(M); //--> OK.

.. note::

   Making view is cheap, it only copies the index systems. Nevertheless
   this can still cause severe overhead in very intense loops. 

   
Compound operators (+=, -=, *=, /=)
-------------------------------------------

The `value classes` and the `view classes` behaves similarly.
We will illustrate it on the `array` class, it is the same for `matrix` and `vector`.

 * **Syntax** 

   The syntax is natural ::

    template<typename RHS> array & operator += (const RHS & X);
    template<typename RHS> array & operator -= (const RHS & X);
    template<typename RHS> array & operator *= (const Scalar & S);
    template<typename RHS> array & operator /= (const Scalar & S);

 * **Behaviour**

   - Similar to assignment, but it makes the operation
   - For matrices, scalar is correctly interpreted as a scalar matrix.


Performance
---------------------------------------------

The performance of such compact writing is as good as "hand-written" code or even better.

Indeed, the operations are implemented with the `expression templates` technique.
The principle is that the result of A+B is **NOT** an array, but a more complex type which stores
the expression using the naturally recursive structure of templates.

Expressions models :ref:`ImmutableCuboidArray` concept.
They behave like an immutable array : they have a domain, they can be evaluated.
Hence they can used *anywhere* an object modeling this concept is accepted, e.g. : 

* array, matrix contruction
* operator =, +=, -=, ...

When an array in assigned (or constructed from) such expression, it fills itself
by evaluating the expression.

This technique allows the elimination of temporaries, so that the clear and readable code::

   Z= A + 2*B + C/2;

is in fact rewritten by the compiler into ::
 
   for (i,j,...) indices of A, B  : 
     C(i,j) = A(i,j) + 2* B(i,j) + C(i,j)/2

instead of making a chain of temporaries (C/2, 2*B, 2*B + C/2...) that "ordinary" object-oriented programming would produce.
As a result, the produced code is as fast as if you were writing the loop yourself,
but with several advantages : 

* It is more **compact** and **readable** : you don't have to write the loop, and the indices range are computed automatically.
* It is much better for **optimization** : 
  
  * What you want is to tell the compiler/library to compute this expression, not *how* to do it optimally on a given machine.
  * For example, since the traversal order of indices is decided at compile time, the library can traverse the data
    in an optimal way, allowing machine-dependent optimization.
  * The library can perform easy optimisations behind the scene when possible, e.g. for vector it can use blas.
  
Note that expressions are lazy objects. It does nothing when constructed, it just "record" the mathematical expression ::

   auto e =  A + 2*B;             // expression, purely formal, no computation is done
   cout<< e <<endl ;              // prints the expression
   cout<< e(1,2) <<endl ;         // evaluates just at a point
   cout<< e.domain() <<endl ;     // just computes its domain
   array<long,2> D(e);            // now really makes the computation and store the result in D.
   D = 2*A +B;                    // reassign D to the evaluation of the expression.


