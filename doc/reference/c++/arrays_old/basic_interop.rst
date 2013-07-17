Interoperability
---------------------------------------------------

* These classes are largely similar, and are interoperable. 
  
  * One can construct a matrix_view from a array, e.g. (providing the rank of the array is 2, of course...).

* Why several classes then ?  The reason is that their algebra (i.e. their behaviour under operations) differ.
  
  For example, a matrix<T> is not really a array<T,2> :
  the multiplication for a matrix is the matrix multiplication, while it is element wise for the array.
  The expression template implements two different algebras, one for arrays, one for matrices and vectors.

TODO : write the example and code the make_matrix_view function....

Example::

 array<double,3> A; matrix<double> M;
 auto res = matrix_view<double> ( A(0,range(),range()) )  + M; 


