Motivation
===========================

.. highlight:: c


This library provides a multi-dimensionnal array library
for numerical computations with the following characteristics/goals : 

* **Simplicity of use**.

  Arrays must be as simple to use as in python (numpy) or fortran.
  This library is designed to be used by physicists, not only by professionnal programmers, 
  We do *a lot* of array manipulations, and we want to maintain *readable* codes.

  Examples::
   
   using namespace triqs::arrays; //  All classes of this library are here. We will omit in the following 
   array<int,3> A(4,2,3);         // creating an array
   A(0,1,2)                       // access an element
   A(range(1,2), range(), 4)      // a partial_view (or slice).

* Various interoperable forms of arrays :   
 
  * array  : the general cuboid n-dimensional array, with **custom memory ordering** at compile time.
  * matrix : a matrix class, with C or Fortran order at compile time.
  * vector : a vector class.

*  **Genericity, abstraction and performance** : 
 
   We want to have simple, readeable code, with the same (or better) speed than manually written low level code.
   Most optimisations should be delegated to the library and the compiler.

   For example, we allow the use of **expression templates** for the algebra of arrays or matrices :: 

    array<int,2> A(4,2),B(4,2),C;
    // fill A, B....
    C = 2* A + B/2

   Expressions templates allows the elimination of temporaries and hence compact and readable code
   with the speed of manually written codes. Thanks to boost::proto library, this can nowadays be achieved
   while maintaining a good compilation time, and with a short and readable code.

* **Compilation time** is (at far as tested) kept under control.
   
* **Complete interoperability with python numpy arrays**.
 
  This library is used a lot with mixed C++/python codes.
  It handles quick conversion between the C++ and python world, e.g. :

   * work on a view of a numpy, 
   * create a array in C++, and return it as a numpy.
   * mix the various kind of arrays transparently in C++ expressions.

* **HDF5** : simple interface to hdf5 library for an easy storing/retrieving into/from HDF5 files.

* **STL compliance**. 
  
  The arrays are compliant with STL (algorithms, std::vector<array<T,N> >, iterators, map, etc...).

* **Simple interface to Blas/Lapack** for matrices, vectors.

* **MPI** : compatibility with boost::mpi interface.




