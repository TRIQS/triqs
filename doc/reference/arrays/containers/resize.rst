.. highlight:: c

.. _arr_resize:

resize 
==================================

* **Synopsis**:

  .. cpp:function:: void resize(size_t, ..., size_t) 

  Resizes the container.




* **Examples** :

.. triqs_example:: ./resize_0.cpp
.. note::
  
   Views cannot be resized.

* **Invalidation** :


  As for std::vector, resize invalidate all pointers to the data of the container
  (they may move in memory, to maintain data contiguity).

  Views are invalidated too: more precisely, because of the memory guarantee provided by the views, 
  the view is still valid (code will not crash), but it does *not* view the container anymore...

  Illustration :

.. triqs_example:: ./resize_1.cpp
.. _arr_resize_ch:

resize_or_check_if_view
==================================

* **Synopsis**:

.. cpp:function:: void resize_or_check_if_view(ArrayType const & A, shape_t)

* If A is a value: resize A
* If A is a view: check that the shape if the shape_t and throw an exception if not.
