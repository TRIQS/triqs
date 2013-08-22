.. highlight:: c

.. _partial_views:

Views 
==============================================================

**Synopsis**:

.. code-block:: c

    template <typename ValueType, int Rank, ull_t OptionsFlags=0, ull_t TraversalOrder=0 > class array_view;
    template <typename ValueType,           ull_t OptionsFlags=0, ull_t TraversalOrder=0 > class matrix_view;
    template <typename ValueType,           ull_t OptionsFlags=0>                          class vector_view;

where triqs::ull_t is the type defined by :

.. code-block:: c

    typedef unsigned long long ull_t;

The view classes have the same template parameters to the regular type.


* A `view` is defined as a view of a restricted portion of the array, matrix or vector.

* The view type of X (= array, matrix, vector) is called X_view, with the same template parameters as the regular type.

* A partial view models the :ref:`MutableCuboidArray`, :ref:`MutableMatrix`, :ref:`MutableVector`, like the corresponding regular type.

* Basically X_view should be understood as a reference to the data of the viewed object, 
  dressed to model the "`MutableX`" concept.

* A view class behaves like the value class when called, put in expression, ... 
  but differs from it by its behaviour under copy and assignment.
  It has a reference semantics : like a pointer or a reference, it does not make a deep copy of the data
  when copied. See :ref:`view_vs_regular` for a detailed discussion of the difference between array and array_view.



======================================================================  =====================================================================================================
Constructors of array_view                                              Comments
======================================================================  =====================================================================================================
array_view(const array_view &)                                          Copy construction (makes a shallow copy)
array_view(const T & X)                                                 T is any type such that X.indexmap() and X.storage() can be used to construct a view.
                                                                        REF to concept here ....
======================================================================  =====================================================================================================

 
Memory management 
------------------------

View classes contains a reference counting system to the memory block they view
(like a std::shared_ptr, but more sophisticated to properly interface to Python numpy).
  
This guarantees that memory will not be dellocated before the destruction of the view.
  
The memory block will be dellocated when its array and all array_view
pointing to it or to a portion of it will be destroyed, and only at that moment.

Example::

   array<int,2> *p = new array<int,2> (2,3); // create an array p
   array_view<int,2> B = *p; // making a view
   delete p; // p is gone...
   B(0,0) = 314; cout<<B<<endl; //  B (and the data) is still alive


.. warning::
 
  TRIQS arrays, and in particular views are NOT thread-safe, contrary to std::shared_ptr.
  This is a deliberate choice for optimisation.


Weak views [Advanced]
^^^^^^^^^^^^^^^^^^^^^^^^

Unfortunately this can not be the end of the story, at least on current C++11 compilers.
It turns out that, in very performance sensitive loops, increasing this tiny 
reference counter can break a lot of optimisations in almost all compilers, including the most
recent ones (gcc 4.8, clang 3.3).

It was therefore decided to introduce `weak views`.

* Weak views are completely similar to "normal" views, except that they do `not` increase the memory
  reference counter.

* Normal views are be implicitely constructed from weak view (the reverse is also true), 
  As soon as you store the weak view into a normal view (e.g. array_view<T,N>)
  the reference counting is again incremented, and the memory guarantee holds.

* Explicit construction of weak views is intentionally not documented here.
  It is designed to be (almost) an implementation detail.

* The () operator returns a weak view `on a modern C++11 compiler`, 
  allowing therefore for better performance on such compiler, in some loops.
  (in particular, `rvalue reference for *this` must be implemented).

* It is however necessary for the advanced user to know about this implementation detail, 
  because in some convolved cases, the memory guarantee may not hold. 
  Example :




