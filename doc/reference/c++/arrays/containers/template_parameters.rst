Template parameters of the containers and views
======================================================

.. _arr_templ_par_opt:

OptionFlags
----------------------------

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
  
.. _arr_templ_par_to:

TraversalOrder
----------------------------

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



