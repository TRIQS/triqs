.. highlight:: c

.. _arr_resize:

resize 
==================================

* **Synopsis**:

  .. cpp:function:: void resize(size_t, ..., size_t) 

  Resizes the container.




* **Examples** :

.. compileblock::

   #include <triqs/arrays.hpp>
   using namespace triqs::arrays;
   int main() { 
    array<double,2> A(2,3);
    A.resize (make_shape (5,5));

    matrix<double,2> M;
    M.resize(3,3); 

    vector<double> V;
    V.resize(10);
   }

.. note::
  
   Views can not be resized.

* **Invalidation** :


  As for std::vector, resize invalidate all pointers to the data of the container
  (they may move in memory, to maintain data contiguity).

  Views are invalidated too : more precisely, because of the memory guarantee provided by the views, 
  the view is still valid (code will not crash), but it does *not* view the container anymore...

  Illustration :

.. compileblock::

   #include <triqs/arrays.hpp>
   using namespace triqs::arrays;
   int main() { 
    array<double,2> A(2,3); A()= 9;
    array_view<double,2> V = A();
    A.resize (make_shape (5,5)); A()=0;
    std::cout<< V<< std::endl;
   }


.. _arr_resize_ch:

resize_or_check_if_view
==================================

* **Synopsis**:

.. cpp:function:: void resize_or_check_if_view(ArrayType const & A, shape_t)

* If A is a value : resize A
* If A is a view : check that the shape if the shape_t and throw an exception if not.
