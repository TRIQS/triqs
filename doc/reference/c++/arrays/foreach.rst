.. highlight:: c

.. _Foreach:

The foreach construct
========================================================

foreach and its variants are a compact and efficient way to
perform some action of the kind 

       *For all elements of the arrays, do something*


While this kind of construct is equivalent to write manually the *for* loops, it has 
several advantages : 

* it is more compact, less error prone (one does not need to specify the bounds in the loop).

* the library orders the for loop in the most efficient way to traverse the memory.
  Hence if one change the TraversalOrder for some arrays, there is no need to change the for loop
  ordering to obtain a better performance, just recompile...

* it is easier to write generic code for array of several dimensions.

foreach
------------

The *foreach* function call a given function *f* successively on the indices of an array *A*,
in the order specified by the TraversalOrder flag of the array.

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
  using the TraversalOrder flag of the array. 

* As a result this is always equally or more optimized than a manually written loop.

Example : 

.. compileblock::

    #include <triqs/arrays.hpp>
    using triqs::arrays::array; 
    int main(){
     array<long,2> A (2,3);
     foreach (A, [&A](size_t i, size_t j) { A(i,j) = i+j;});
     std::cout<<" A "<< A << std::endl;
    } 

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
  using the TraversalOrder flag of the array. 

.. compileblock::

    #include <triqs/arrays.hpp>
    using triqs::arrays::array; 
    int main(){
     array<long,2> A (2,3);
     assign_foreach (A, [](size_t i, size_t j) { return i+j;});
     std::cout<<" A "<< A << std::endl;
    } 

.. note::
   Cf the note of the *foreach* function.
