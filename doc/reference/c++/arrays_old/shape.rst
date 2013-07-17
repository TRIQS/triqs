
.. _Shape:

Shape, resize 
==================================

Shape
--------------------

* array, matrix and vector have a method shape() that returns a `shape_type` object
  i.e. a mini_vector<size_t,rank>

* Tutorial::

   array<double,2> A(2,3);
   A.shape()[0] == 2;
   A.shape()[1] == 3;

Resize
--------

* The `value class` array, matrix and vector can be resized::

   array<double,2> A(2,3);
   A.resize ( make_shape (5,5) )
     
   matrix<double,2> M;
   M.resize( 3,3); 

   vector<double> V;
   V.resize(10);


