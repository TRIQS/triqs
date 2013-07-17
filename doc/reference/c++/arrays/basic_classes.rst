Basic classes : array, matrix and vector and their views
=================================================================

* The library provides several interoperable forms of arrays : 

   * array : general (rectangular) `N`-dimensionnal array; models :ref:`HasImmutableArrayInterface` concept.
   * matrix : models the :ref:`MutableMatrix` concept.
   * vector : models the :ref:`MutableVector` concept.

  and the corresponding view classes : array_view, matrix_view, vector_view.

* All these classes contains basically two things : 

   * a *storage*, i.e. a (shared) pointer to the data in memory.
   * an *indexmap*, i.e. an object that encode the index systems and can map it to the memory.

* They differ by their behaviour : 
 
   * array, matrix, vector are *values* with value semantics, while the X_view classes 
     are views, with reference semantics, see below.
   * array form an array algebra, where operation are done element-wise, while matrix and vector 
     for the usual algebra and vector space of linear algebra.

*  These classes are largely interoperable, as explained below : it is easy and quick to take a
   matrix_view of an array, or vice versa.

*  The classes haves similar template parameters : 

   .. code-block:: c

     typedef unsigned long long ull_t;

     template <typename ValueType, int Rank, ull_t OptionsFlags=0, ull_t TraversalOrder=0 > class array;
     template <typename ValueType, int Rank, ull_t OptionsFlags=0, ull_t TraversalOrder=0 > class array_view;
  
     template <typename ValueType, ull_t OptionsFlags=0, ull_t TraversalOrder=0 > class matrix;
     template <typename ValueType, ull_t OptionsFlags=0, ull_t TraversalOrder=0 > class matrix_view;
  
     template <typename ValueType, ull_t OptionsFlags=0> class vector;
     template <typename ValueType, ull_t OptionsFlags=0> class vector_view;

   where triqs::ull_t is the type defined by :

    .. code-block:: c

      typedef unsigned long long ull_t;

Template parameters
----------------------------

 ============================    ==================================  ==========================  ====================================================================
 Template parameter              Accepted type                       Access in the class         Meaning                                    
 ============================    ==================================  ==========================  ====================================================================
 ValueType                       normally a scalar, but any default  value_type                  The type of the element of the array           
                                 constructible type (?).                                    
 Rank                            int                                 rank                        The rank of the array 
 OptionsFlags                    unsigned long long                                              Compile time options, see below.
 TraversalOrder                  unsigned long long                                              Compile time options, see below.
 ============================    ==================================  ==========================  ====================================================================

* Rank is only present for array, since matrix have rank 2 and vector rank 1.

* OptionFlags is a series of flags determining various options at compile time.
  The possible flags are accessible via a constexpr ull_t in triqs::arrays or a macro : 
 
  ======================== =======================================  
  Macro                    constexpr equivalent
  ======================== =======================================  
  BOUND_CHECK              triqs::arrays::BoundCheck
  TRAVERSAL_ORDER_C        triqs::arrays::TraversalOrderC
  TRAVERSAL_ORDER_FORTRAN  triqs::arrays::TraversalOrderFortran
  DEFAULT_INIT             triqs::arrays::DefaultInit
  ======================== =======================================  

  
  Defaults can be modified with the macros : 

  * `TRIQS_ARRAYS_ENFORCE_BOUNDCHECK`   : enforce BoundCheck by default (slows down codes ! Use only for debugging purposes).
  * `TRIQS_ARRAYS_ENFORCE_INIT_DEFAULT` : init all arrays by default [ NOT IMPLEMENTED]
  

* TraversalOrder is a coding of the optimal ordering of indices, given by a permutation
  evaluated at **compile time**. 
  The traversal of the arrays (iterators, foreach loop) will be written and optimised for this 
  order.

  The default (0) is understood as regular C-style ordering (slowest index first).

  Note that an array can use any index ordering in memory, and that decision is take at run time
  (this is necessary for interfacing with python numpy arrays, see below). 
  The code will be correct for any order, but optimised for the TraversalOrder.
  
  For a few very specials operations (e.g. regrouping of indices), the indices ordering in memory and TraversalOrder
  must coincide. This will be explicitely said below. By default, it is not necessary.

  The permutation P encodes the position of the index :  P[0] is the fastest index, P[rank - 1]  the slowest index 
  (see examples below).

  TraversalOrder is not present for vector since there is only one possibility in 1d.


* Examples will be given in the next paragraph, with constructors.

