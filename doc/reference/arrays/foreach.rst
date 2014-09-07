.. highlight:: c

.. _Foreach:

Loops: the foreach constructs
========================================================

foreach and its variants are a compact and efficient way to
perform some action of the kind 

       *For all indices of the array, do something*


While this kind of construct is equivalent to write manually the *for* loops, it has 
several advantages: 

* it is more compact, less error prone (one does not need to specify the bounds in the loop).

* The library orders the loops in way specified by the template parameters TraversalOrder (by default, the standard C order), 
  to obtain the most efficient way to traverse the memory.

* it is easier to write generic code for array of several dimensions.

foreach
------------

The *foreach* function call a given function *f* successively on the indices of an array *A*,
in the order specified by the TraversalOrder of the array.

* Synopsis::
      
    template <typename ArrayType, typename Function>
    void foreach (ArrayType const & A, Function F);   

  * A is an array/matrix/vector or the corresponding view.
  * The template is enabled iif ImmutableArray<ArrayType>::value == true
  * F is a function with the following synopsis ::

      F(size_t ... indices)

* The foreach algorithm is equivalent to ::

   for (i,j,k...) F(i,j,k...)

* The for loop are automatically organised to optimize the traversal order of A
  using the TraversalOrder of the array. 

* As a result this is always equally or more optimized than a manually written loop.

Example: 

.. triqs_example:: ./foreach_0.cpp
.. note::
   You *can* pass a std::function as Function, but it is not recommended in critical parts of the code.
    
   The indirection caused by std::function at each call may lead to big performance penalty.
   
   The call to  lambda, or a custom callable object will on the other hand by inlined.

assign_foreach 
----------------

assign_foreach is a simpler form that assigns the return value of the function to the array elements.
Note that using the lazy expression is usually a lot simpler (except when you already have the function ready).

Synopsis::
     
   template <typename ArrayType, typename Function>
   void assign_foreach (ArrayType const & A, Function F);   

  * A is an array/matrix/vector or the corresponding view.
  * The template is enabled iif ImmutableArray<ArrayType>::value == true
  * F is a function with the following synopsis ::

    F(size_t ... indices)

* The assign_foreach algorithm is equivalent to ::

   for (i,j,k...) A(i,j,k...) = F(i,j,k...)

* The for loop are automatically organised to optimize the traversal order of A
  using the TraversalOrder of the array. 

.. triqs_example:: ./foreach_1.cpp
.. note::
   Cf the note of the *foreach* function.
