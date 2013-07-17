.. highlight:: c

View  classes and their constructors 
==============================================================================

* A view class (array_view, matrix_view, vector_view) is used to present a
  partial (or complete) view of an array, matrix, vector. 
  Basically it should be understood as a reference to the data of the viewed object, 
  dressed to model the :ref:`MutableArray` concept.

* A view class behaves like the value class when called, put in expression, ... 
  but differs from it by its behaviour under copy and assignment.
  It has a reference semantics : like a pointer or a reference, it does not make a deep copy of the data
  when copied.

* There are two ways to make a view : 

  * explicit construction, by calling a constructor or the make_view function
  * automatic construction : views are the results of slicing the array

* In the following, we will discuss only array_view, since matrix_view and vector_view
  behaves exactly in a similar way.

Explicit construction of a view 
----------------------------------------

======================================================================  =====================================================================================================
Constructors of array_view                                              Comments
======================================================================  =====================================================================================================
array_view(const array_view &)                                          Copy construction (makes a shallow copy)
array_view(const T & X)                                                 T is any type such that X.indexmap() and X.storage() can be used to construct a view.
                                                                        REF to concept here ....
======================================================================  =====================================================================================================

The function make_view can also be used to make a view.

Example ::

   array<int,2> A(2,2);
   array_view<int,2> V(A); 
   auto V2 = make_view(A);

   struct view_keeper {
     array_view<int,2> V;

Memory management 
------------------------

View classes contains a reference counting system to the memory block they view
(i.e. a std::shared_ptr).
  
This guarantees that memory will not be dellocated before the destruction of the view.
  
The memory block will be dellocated when its array and all array_view
pointing to it or to a portion of it will be destroyed, and only at that moment.

Example::

   array<int,2> *p = new array<int,2> (2,3); // create an array p
   array_view<int,2> B(*p); // making a view
   delete p; // p is gone...
   B(0,0) = 314; cout<<B<<endl; //  B (and the data) is still alive

NB : This means that constructing a new view is cheap but still : document here the question of proxy ??


