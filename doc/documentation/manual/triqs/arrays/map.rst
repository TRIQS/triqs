.. highlight:: c

.. _arr_map_fold:

Functional constructs: map & fold
###########################################

Two standard functional constructs are provided: 

* *map* that promotes a function acting on the array element to an array function, acting 
  element by element.

* *fold* is the reduction of a function on the array. 

.. _map:

map
========================================================
* **Purpose** :

  map promotes any function into an `array function`, acting term by term.

* **Synopsis** ::

    template<class F> auto map (F f);

  If `f` is a function, or a function object :: 
   
    T2 f(T1)

  Then map(f) is a function::
  
     template<ImmutableCuboidArray A> auto map(f) (A const &)
   
  with: 
     * A::value_type == T1
     * The returned type of map(f) models the :ref:`ImmutableCuboidArray` concept

       * with the same domain as A
       * with value_type == T2

* **Example**: 

.. literalinclude:: ./map_0.cpp

fold
====

* **Purpose** :
  fold implements the folding (or reduction) on the array.

* **Syntax** :

  If `f` is a function, or a function object of synopsis (T, R being 2 types) ::

       R f (R , T)
  
  then  ::

    auto F = fold(f);

  is a callable object which can fold any array of value_type T.

  So, if 
  
   * A is a type which models the :ref:`ImmutableCuboidArray` concept
     (e.g. an array , a matrix, a vector, an expression,  ...)

   * A::value_type is T

  then ::

    fold (f) ( A, R init = R() ) = f(f(f(f(init, a(0,0)), a(0,1)),a(0,2)),a(0,3), ....) 
          
  Note that: 
   
   * The order of traversal is the same as foreach.
   * The precise return type of fold is an implementation detail, depending on the precise type of f, 
     use auto to keep it.
   * The function f will be inlined if possible, leading to efficient algorithms.
   * fold is implemented using a foreach loop, hence it is efficient.

* **Example**: 
  
  Many algorithms can be written in form of map/fold.

  The function :ref:`arr_fnt_sum` which returns the sum of all the elements of the array is implemented as ::

   template <class A>
   typename A::value_type sum(A const & a) { return fold ( std::plus<>())  (a); }

  or the Frobenius norm of a matrix, 
  
  .. math::  
      \sum_{i=0}^{N-1}  \sum_{j=0}^{N-1} | a_{ij} | ^2 

  reads :

  .. literalinclude:: ./map_1.cpp

  Note in this example: 
   
   * the simplicity of the code
   * the genericity: it is valid for any dimension of array.
   * internally, the library will rewrite it as a series of for loop, ordered in the TraversalOrder of the array
     and inline the lambda.








