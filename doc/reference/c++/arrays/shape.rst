.. _Shape:

Shape, resize 
==================================

Lengths
----------


Shape
--------------------

* array, matrix and vector have a method shape() that returns a `shape_type` object
  i.e. a mini_vector<size_t,rank>. DOCUMENT THIS ?

* Example::

   array<double,2> A(2,3);
   A.shape()[0] == 2;
   A.shape()[1] == 3;

Resize
--------

* The value classes array, matrix and vector can be resized.

* **Synopsis**:

  .. cpp:function:: void resize(size_t, ..., size_t) 

* **Examples** ::

   array<double,2> A(2,3);
   A.resize ( make_shape (5,5) )
     
   matrix<double,2> M;
   M.resize( 3,3); 

   vector<double> V;
   V.resize(10);

* Views can not be resized.

resize_or_check_if_view
----------------------------

.. cpp:function:: void resize_or_check_if_view(ArrayType const & A, shape_t)

* If A is a value : resize A
* If A is a view : check that the shape if the shape_t and throw an exception if not.
