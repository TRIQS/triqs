Memory management
========================

.. highlight:: c

* `View classes` contains a **reference counting** system to the memory block they view.
  
  This guarantees that memory will not be dellocated before the destruction of the view.
  
  The memory block will be dellocated when its array and all array_view
  pointing to it or to a portion of it will be destroyed, and only at that moment.

  Example::

   array<int,2> *p = new array<int,2> (2,3); // create an array p
   array_view<int,2> B(*p); // making a view
   delete p; // p is gone...
   B(0,0) = 314; cout<<B<<endl; //  B (and the data) is still alive

* **Python compatibility**
  
  The reference counting system is *compatible* with the Python reference counting (but distinct),
  if you compile with python support of course.

  As long as you write pure C++ code, you are basically using a shared_ptr to your data block.
  No python is involved.

  But, if you return your view into a numpy array in python, ownership of your data 
  is automatically transfered to the python interpreter::

   boost::python::object f() { 
   array<int,2> res; 
   /// I compute res
   return boost::python::object(array_view<int,2>(res)); // Cf section for convertion...
   }

  The interpreter then take the responsability of destroying the data when needed (meaning here, long after f has returned,
  when the python object returned will be cleaned).

 
