.. highlight:: c

.. _arr_view_memory:

View memory management 
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

Unfortunately this cannot be the end of the story, at least on current C++11 compilers.
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

* The () operator returns a weak view `on a fully compliant C++11 compiler` (in particular, `rvalue reference for *this` must be implemented)
  allowing therefore for better performance on such compiler, in some loops.
  Older supported compiler will therefore generate code with lower performances.  

* It is however necessary for the advanced user to know about this implementation detail, 
  because in some convolved cases, the memory guarantee may not hold. 
  Example:

.. warning::
   
 TO BE WRITTEN





