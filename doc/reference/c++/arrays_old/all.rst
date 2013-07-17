.. highlight:: c

array and array_view
============================

array and array_view are the class for standard d-dimensional cuboid array
and the corresponding view.

Template parameters
----------------------------

* The class has four template parameters (same for array_view).

 .. code-block:: c

   array<ValueType,                   // The type of the element of the array
        Rank,                         // int : the rank of the array
        IndexOrderTag=Tag::C,         // The ordering in memory : can be Tag::C, Tag::Fortran or a permutation 
        StorageTag=Tag::shared_block  // The storage : Tag::shared_block or Tag::numpy
        >


 ============================    ==========================  =======================================
 Template parameter                Access in the class       Meaning                                    
 ============================    ==========================  =======================================
 ValueType                       value_type                  The type of the element of the array           
 Rank                            rank                        The rank of the array *[int]*
 IndexOrderTag                   indexmap_type               The ordering in memory : can be Tag::C, Tag::Fortran or a permutation 
 StorageTag                      storage_type                The storage : Tag::shared_block or Tag::numpy
 ============================    ==========================  =======================================


* Various IndexOrderTag are possible : 

  =================    ==================================================================================================== 
  IndexOrderTag        Meaning
  =================    ==================================================================================================== 
  Tag::C               C-style order *[default]*
  Tag::Fortran         Fortran-style order
  P                    - P is a permutation 
                       - Determined by a permutation P at compile time.  Explain here the permutation, the convention.
  =================    ==================================================================================================== 


* Two possible storages : 

 ==================  ============================================================================
 StorageTag          Meaning
 ==================  ============================================================================
 Tag::shared_block   a (shared_ptr on a) C++ block *[default]*
 Tag::numpy          stored in a numpy array, in which case the array is also a numpy array 
                      and read numpy, be returned as numpy, sliced into a numpy, etc...
 ==================  ============================================================================


.. _array_constructors:

Constructors
-----------------

Intentionally, array and array_view have only a few constructors : 

==========================================  ===========================================================================================
Constructors of array                       Comments
==========================================  ===========================================================================================
array()                                     - empty array of size 0
array(size_t, ...., size_t)                 - from the dimensions
array(cuboid_domain<rank> const &)          - a new array with the corresponding cuboid 
array(const array &)                        - copy construction
array(const T & X)                          - Type T models the :ref:`HasImmutableArrayInterface` concept.
                                            - X must have the appropriate domain (checked at compile time).
                                            - Enabled iif has_immutable_array_interface<T>::value == true.
                                            - Constructs a new array of domain X.domain() and fills it with evaluation of X.  
==========================================  ===========================================================================================

======================================================================  =======================================================================================
Constructors of array_view                                              Comments
======================================================================  =======================================================================================
array_view(indexmap_type const & I, S_type const &)                     from a couple of indexmap I and storage of type S_type                                  
array_view(const T & X)                                                 T is any type such that X.indexmap() and X.storage(). Models ISP ...
======================================================================  =======================================================================================

array_view are typically constructed by slicing (Cf below).

* Examples :: 

   array<int,2> A(10,2);                    
   array<int,2,Tag::Fortran> Af (2,2);    

   //Higher dim, custom order : 
   array<long, 3, Permutations::permutation<2,1,0> > A0 (2,3,4);
   array<long, 3, Permutations::permutation<0,1,2> > A1 (2,3,4);
   array<long, 3, Permutations::permutation<1,0,2> > A2 (2,3,4);
   array<long, 3 > A3 (2,3,4);
   array<long, 3, Tag::Fortran > A4 (2,3,4);
       

Access to data, domain, simple evaluation, ...
--------------------------------------------------------

array, array_view, matrix, matrix_view, vector, vector_view model HasImmutableArrayInterface.

 
Assignment & Copy
--------------------

Every classes comes in two flavors: C and C_view (with C = array, matrix, vector, etc...).
These two flavors differ in the way they handle their data in construction, copy construction, assignement.
Basically, C owns its data, while C_view if only a view.

array, matrix, vector
^^^^^^^^^^^^^^^^^^^^^^^^

They own their data. In many aspects, they are similar to  like std::vector.

* The data are contiguous in memory.
* Constructors and copy constructors all create a new memory block. If needed, they
  make a *true* copy of the data.
* The assignment operator may create a new Storage if size do not match.
* As a result, /pointers to the data/ and reference to the storage are invalid after assignment.
* They can be resized, again invalidating all references.
   
array_view, matrix_view, vector_view
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These classes do not own their data, but only present a view of them.

* The data may not be contiguous in memory  (e.g. if the view is the result of a slice).
* Constructors only make another view of the data.
* They *never* copy data, so they are quite quick. 
  In particular, copy constructor makes shallow copy (i.e. return another view).
* The assignement operator just copy data into the view. Behaviour is undefined if the 
  size of the view is too small (define the macro ARRAY_CHECK for dynamical debugging checks). 
* Pointers to data taken from the views are still valid after assignement.
* Views can be not be resized.

.. warning:: **Memory management**

  Views carry a reference to the memory block they view, 
  which guarantees that memory will not be 
  dellocated before the destruction of the view.
  Indeed, the Storage types implement incorporated a reference counting mechanism, 
  either using boost::shared_ptr for the C++ arrays, or using the python references
  for the numpy storage.
  The memory block will be dellocated when its array and all array_view
  pointing to it or to a portion of it will be destroyed, and only at that moment.

Examples::

  array<int,Matrix> *A = new array<int,Matrix> (Matrix(2,3)); // create an array A
  array_view<int, Matrix> B(*A); // making a view
  delete A; // A is gone...
  cout<<B<<endl; // ok, but B (and the data) is still alive

Operator =
------------

array, matrix, vector
^^^^^^^^^^^^^^^^^^^^^^^^

//.. cpp:member::
    template<typename RHS> array & operator=(const RHS & X);

 * RHS models HasImmutableArrayInterface. 
 * array is first resized to have a domain X.domain(), and then filled
   with the evaluation of X (e.g. a copy if X is an array, computing the value if X is an expression).


array_view, matrix_view, vector_view
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//.. cpp:function::
   template<typename RHS> array_view & operator=(const RHS & X);

 * RHS models HasImmutableArrayInterface [ or less  ? : RHS can be evaluated in the domain_type::value_type, no domain needed.].
 * Dimension of the view must match or behaviour is undefined.

Iterators and interaction with STL containers and algorithms
----------------------------------------------------------------

STL compliant iterators, hence STL algorithms work...

Examples::

 array<long,2> A (2,3);
 
 // first print the index generator
 for (array<long,2>::indexmap_type::domain_type::generator it = A.indexmap().domain().begin(); !it.at_end(); ++it)
  cout<<"  "<<*it<<endl;

 //A(i,j) = i + 10*j
 for (array<long,2>::iterator it = A.begin(); !it.at_end(); ++it) 
  { *it =it.indices().get<0>() + 10 *it.indices().get<1>() ; }

 int u=0;
 for (array<long,2>::iterator it = A.begin(); !it.at_end(); ++it,++u) { *it =u; }

 array<long,2> A (2,3);
 std::vector<array<long,2> > VV; VV.push_back(A);
 map<string, array<long,2> > MAP; MAP["1"] = A;

 // Trying to put a vector in an array
 std::vector<int> V (10);
 array<int,1 > B(V.size()), C(V.size());
 for (unsigned int i =0; i<10; ++i) V[i] = 10+i;

 std::copy(V.begin(),V.end(),B.begin()); 
 std::copy(B.begin(),B.end(),V.begin()); 
 cout<<" Number of elements <25 : "<< std::count_if(B.begin(), B.end(),te)<<endl;
 cout<<" max(B) "<< *std::max_element(B.begin(),B.end())<<endl;
 std::replace_if (B.begin(), B.end(), te, 0);
 std::swap(B,C);


Slicing (or partial view)
-------------------------------------------------

Slicing or partial view consists in presenting a view of a sub-part of the array, e.g.
Examples::

 array<long,2> A (2,3);
 array_view<long,1>  SL( A(range(0,2),0));
 array_view<long,1>  SL2( A(1,range(0,2)));

 
It is the standard way to produce a view.

NB : 

* we use here the python convention: range(0,3) is 0:3, i.e. 0,1,2 NOT 0,1,2,3.
* Todo : in fact we should wrap the range to python::slice for interoperability with python.

Serialization
------------------------------------------------- 

* Boost.serialization
* Boost.mpi

Examples::

 array<long,2> A (2,2), B(2,2),C(2,2);
 boost::mpi::reduce (world, A,C, std::plus<array<long,2> >(),0);


* HDF5 (ALPS), eg.

Examples::

 array<long,2> A (2,3),B,vc;
 array<long,2,Tag::Fortran> Af,Bf,vf;

 alps::hdf5::oarchive ar1("data.h5");
 ar1 << alps::make_pvp("Tableau", A);
 ar1 << alps::make_pvp("Tableau2", Af);
 ar1 << alps::make_pvp("Tableau_view",  A(range(),range(1,3)));
 
 alps::hdf5::iarchive ar2("data.h5");
 ar2 >> alps::make_pvp("Tableau", B);
 ar2 >> alps::make_pvp("Tableau", Bf);
 ar2 >> alps::make_pvp("Tableau_view", vc);
 ar2 >> alps::make_pvp("TableauC",C);
 

blas/lapack interface
-------------------------------------------------

* matrix, vector and their views are interfaced with blas/lapack, via boost::numerics::bindings.
* If needed (for a view), a temporary (and silent) copy is made to reorganize the 
data before calling blas/lapack (impl: cache class).
Of course, performance is altered, but code is simple...

Examples::

 namespace blas = boost::numeric::bindings::blas;
 namespace lapack = boost::numeric::bindings::lapack;

 triqs_arrays::vector<std::complex<double> >  V(5),V2(5);
 triqs_arrays::vector <double> V3(2);
 triqs_arrays::matrix<double,'F' > M1(2,2), M2(2,2), M3(2,2);

 blas::axpy(2.0,V,V2);
 blas::gemm(1.0,M1, M2, 1.0, M3);
 blas::ger(1.0,V3,V3,M2);

 // invert
 triqs_arrays::vector <int> ipiv(2);
 lapack::getrf(M1, ipiv);
 lapack::getri(M1, ipiv);

Transparent use of python arrays
-------------------------------------------------

* If the storage is Tag::numpy, the memory block is allocated/viewed through the numpy interface.
* One can mix arrays with any storage in expression (they have the same concepts).
* boost python converters are enable for those arrays into numpy and their views [impl :broken for views].


Expression
-------------------------------------------------

Simple expressions are made using boost.proto.
Examples :: 

 array<long,2> A (2,2), B(2,2),C;
 C= A + 2*B;
 array<long,2> D( A+ 2*B);

 // or even in C++0x : 
 auto e =  A + 2*B;           // expression, purely formal
 array<long,2> D(e);          // really makes the computation
 cout<< e <<endl ;            // prints the expression
 cout<< e(1,2) <<endl ;       // evaluates just at a point
 cout<< e.domain() <<endl ;   // just computes the domain

 array<long,2> A (2,2), B(2,2),C(2,2);
 C= A + 2*B;
 C= std::plus<array<long,2> >()(A,B);
 C = A + Transpose(B);         // Transpose(X) returns a lazy object that models HasImmutableArrayInterface. 
 C = A + Transpose(B + B);     // X can also be an expression...
 C = Transpose(B);             //
 array<double,2> F( 0.5 * A);  // Type promotion is automatic

 // non square
 array<long,2> R(2,3),Rt(3,2);
 cout<<" R = "<< array<long,2>(Transpose(R)) <<endl;

 // mapping any function 
 C =  map_expr(&sqr,A);
 cout<<" C = "<< map_expr(&sqr,A,"SQR")<<" = "<<C<<endl;

 // matmul as expression Oi are 'C' or 'F'
 matrix<double,O1> M1(2,2); matrix<double,O2> M2(2,2); matrix<double,O3> M3;
 // The central instruction : note that matmul returns a lazy object 
 // that has ImmutableArray interface, and defines a specialized version assignment
 // As a result this is equivalent to some matmul_with_lapack(M1,M2,M3) : there is NO intermediate copy.
 M3 = matmul(M1,M2);

See expression.hpp.
At the moment, only +,-, * and / by scalar are implemented.

An expression models HasImmutableArrayInterface, i.e. :
 
* It has a domain (computed from the expression)
* It can be evaluated.

It is then easy to mix them with other objects,
that model the same concept. See e.g. expr2.cpp (map_expr) for examples.


* *Multiplication* : 
not decided, since it is not the same for array or matrices.
Two choices : 

 * Do not add * for array, matrices (use matmul e.g.) and allow mixing array, matrix
   e.g. add an array<int,2> and a matrix <int>

 * Add the *, but then do different expression for array and matrix/vector,
   then one can not mix them.
   In that case, it is however trivial to say e.g. M + matrix_view<int>(A) if A is an array.


