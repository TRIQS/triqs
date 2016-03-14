.. highlight:: c

.. _arr_call:

Operator()
==================================

**Synopsis** ::

  value_type const & operator()(size_t ...) const                            (1a)  
  value_type       & operator()(size_t ...)                                  (1b) 
  
  view_type          operator()() const                                      (2a) 
  view_type          operator()( size_t | range | ellipsis ) const           (2b) 

  `clef expression`  operator()( `at least a lazy argument` ) const          (3) 

This is valid for both the container (e.g. array), and the view (e.g. array_view).


.. _arr_element_access:

(1) Element access 
---------------------------------

Following the concept :ref:`ImmutableCuboidArray`, the form (1) is an access to the elements.

It must be called with exactly `rank` size_t (or it is a compile time error).

Example
^^^^^^^^^

.. triqs_example:: ./call_0.cpp
Another ::

   A(1, range(0,2) )                      // 1d slice
   A(1, range())                          // 1d slice taking all the second dim
   
   A(range(0,10,2), range(0,10,2))        // a 2d slice viewing every each elements with even coordinates.
   
   array_view<T,1>  SL =  A(0,range(0,3)); // naming the view. No data copied here !
   array_view<T,1>  SL ( A(0,range(0,3))); // same thing !



.. _arr_making_view:
    
(2) Building a view
---------------------------------

  
When the arguments contains at least one :ref:`range` or one :ref:`ellipsis<arr_ellipsis>`, and no placeholder (see 3)),
the return type is a (partial) view of the container.

The special case (2a) (no argument) returns a complete view of the object
(equivalent to view_type(* this)).

The return type of the () operator is: 
  
  * Partial views of array or array_view return a array_view.
  * Partial views of vector or vector_view return a vector_view.
  * 2d partial views of matrix or matrix_view return a matrix_view.
  * BUT: (1d) partial view of matrix or matrix_view return a vector_view.


Example
^^^^^^^^^^^^

.. triqs_example:: ./call_1.cpp
.. toctree::
   :hidden:

   /cpp2doc_generated/triqs/arrays/range
   ellipsis

.. highlight:: c

.. _arr_lazy:

(3) Interaction with clef expressions 
-------------------------------------------------


* The containers and their views can be used with the triqs::clef library: 

* Using the clef library offers a quick and efficient way to fill an array with multiple advantages: 

  * It is simpler and more readeable than a series of for loops.
  * It is usually more optimal since the for loops are automatically written in the TraversalOrder of the 
    array.

* NB: the expression can be (and are) inlined by the compilers...

* **Example**: 

.. triqs_example:: ./call_2.cpp
.. note:: 
   The syntax uses a <<, not = since the array is not assigned to an expression
   but filled by the evaluation thereof.



  


