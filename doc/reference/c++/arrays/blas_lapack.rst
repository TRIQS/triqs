.. highlight:: c

Linear algebra 
===============================================

.. warning::

   Doc still largely to be written and updated....

Several standard linear algebra operations are provided for the matrix and vector.
Note however that matrix and vector are not the main purpose of this library, 
hence the linear algebra support is less extended than other purely matrix library, 
like e.g. Eigen.

The computation are done when possible by calling lapack and blas.
In some important cases (like matrix product), a slow but generic version is also
provided for type that lapack do not treat (e.g. matrix of int, matrix of custom objects)
but be aware that these are not optimized for performance and should be used only in 
non critical part of the codes.

matrix product
--------------------

The * operator map the matrix x matrix and matrix x vector product.

Example : matrix * matrix and * vector ...

..  
   compileblock::
   #include <triqs/arrays.hpp>
   using triqs::arrays::matrix; using triqs::clef::placeholder;
   int main() {  
    // declare an init 2 matrices 
    placeholder<0> i_; placeholder<1> j_;
    matrix<double> A (2,2), B(2,2), C; 
    A(i_,j_) <<  i_ + j_ ; B(i_,j_) <<  2*i_ + j_ ;
    C= A*B;
    std::cout<< " C = " << C<< std::endl;   
   }
 
For types that lapack do not use, a generic version of the matrix product is provided.
(same syntax, the dispatch is made at compile time depending of the type of the matrices).


Matrix inversion
----------------------

The inverse function return a lazy inverse of any object which has ImmutableMatrix concept
and can therefore be mixed with any other matrix expression.

Example : TO BE WRITTEN


LU decomposition
----------------------

Done. doc to be written

Diagonalization 
-------------------
Done. doc to be written

SVD decomposition
-------------------

To be done 

Interface with Eigen
------------------------

To be implemented. Only possible for when order is known at compile time.


