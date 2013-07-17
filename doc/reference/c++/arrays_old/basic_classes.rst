Basic classes and constructors 
=================================================================

The library provides several interoperable forms of arrays : array, matrix and vector.


.. toctree::
   :maxdepth: 2

   arrays
   matrix
   vector
   options
   memory_management 
   basic_interop


.. note:: Why several classes ?  

   Because their algebra differ, e.g. a matrix<T> is not really a array<T,2> :
   the multiplication for a matrix is the matrix multiplication, while it is element wise for the array.


