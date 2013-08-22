.. highlight:: c

array, matrix & vector 
===========================================

**Synopsis**:

.. code-block:: c

    template <typename ValueType, int Rank, ull_t OptionsFlags=0, ull_t TraversalOrder=0 > class array;
    template <typename ValueType,           ull_t OptionsFlags=0, ull_t TraversalOrder=0 > class matrix;
    template <typename ValueType,           ull_t OptionsFlags=0>                          class vector;

where triqs::ull_t is the type defined by :

.. code-block:: c

  typedef unsigned long long ull_t;


* The library provides three basic containers : 

   * array : general (rectangular) `N`-dimensionnal array; models :ref:`MutableCuboidArray` concept.
   * matrix : models the :ref:`MutableMatrix` concept.
   * vector : models the :ref:`MutableVector` concept.

  and the corresponding view classes : array_view, matrix_view, vector_view (Cf :ref:`partial_views`).

* The matrix and vector are very similar to an array of dimension 2 and 1 respectivily,
  except for their algebra. Array form an array algebra, where operation are done element-wise, while matrix and vector 
  form the usual algebra and vector space of linear algebra.

* These classes are largely interoperable, as explained below : it is easy and quick to take a
  matrix_view of an array, or vice versa.

Template parameters
----------------------------

 ============================    ==============================  ==========================  ====================================================================
 Template parameter              Accepted type                   Access in the class         Meaning                                    
 ============================    ==============================  ==========================  ====================================================================
 ValueType                       any regular type (typically a   value_type                  The type of the element of the array           
                                 scalar).                                               
 Rank                            int                             rank                        The rank of the array 
 OptionsFlags                    ull_t                                                       Compile time options, see below.
 TraversalOrder                  ull_t                                                       Compile time options, see below.
 ============================    ==============================  ==========================  ====================================================================

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
  

* TraversalOrder is a coding of the optimal ordering of indices, given by a permutation
  evaluated at **compile time**. 
  The traversal of the arrays (iterators, foreach loop) will be written and optimised for this 
  order.

  The default (0) is understood as regular C-style ordering (slowest index first).

  Note that an array can use any index ordering in memory, and that decision is take at run time
  (this is necessary for interfacing with python numpy arrays, see below). 
  The code will be correct for any order, but optimised for the TraversalOrder.
  
  For a few very specials operations (e.g. regrouping of indices), the indices ordering in memory and TraversalOrder
  must coincide. This will be explicitely said below. By default, it is not necessary (but can be suboptimal).

  The permutation P encodes the position of the index :  P[0] is the fastest index, P[rank - 1]  the slowest index 
  (see examples below).

  TraversalOrder is not present for vector since there is only one possibility in 1d.


Member types
-----------------

=================== ======================================
Member type         Definitions
=================== ======================================
value_type          ValueType   
view_type           The corresponding view type

=================== ======================================


Member functions
---------------------

=============================================   ==============================================
Member function                                 Meaning
=============================================   ==============================================
:ref:`(constructor)<arr_reg_constr>`   
(destructor)
:ref:`operator =<arr_reg_assign>`               assigns values to the container       
:ref:`operator ()<arr_reg_call>`                element of access/views/lazy expressions 


begin/cbegin                                    returns iterator to the beginning      
end/cend                                        returns iterator to the end         
:ref:`resize<arr_resize>`                       resize the container 

=============================================   ==============================================


.. toctree::
   :hidden:

   reg_constructors
   reg_assign
   call 
   resize
   STL 

Non-member functions
------------------------


=============================================   ==============================================
Member function                                 Meaning
=============================================   ==============================================
:ref:`swap<arr_swap>`                                 
:ref:`deep_swap<arr_deep_swap>`                                 

=============================================   ==============================================


.. toctree::
   :hidden:

   swap

