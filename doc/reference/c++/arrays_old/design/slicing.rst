.. _DesignSlicing:

Slicing
=============================================================

In this section, IM denotes some IndexMaps.

* We refer here to any partial view of the arrays as "slicing", i.e. subarrays, true slices, etc...
  A slicing is any (meta)-function that take an indexmap and return another one.
  
 * It is a meta-function that computes the type of the resulting indexmap
 * It is a function that computes the resulting indexmap.

* The array/matrix/vector classes and their views, when called with the () operator, will : 

 * forward all the arguments and their types to IndexMaps::slice, to compute the new indexmap IM2.
 * If IM2 is of dimension 0,  return a value_type & or const & (it is a simple number, not an array).
 * Otherwise : return a new view, made of IM2 and the same data as for the original object.

* Possible slices are defined by the IndexMap type.
  Slicing an class C with IndexMap I1 produces a class C2_view, with IndexMap I2,
  i.e. a new sliced IndexMap on the same data.

* **Examples** : 
 * array and array_view can be sliced : 
``  
   array<T,2> A(10,10);               : defines an array
   A(1,2)                             : element access.
   A ( 1, range(0,2) )                : 1d slice
   A( range(0,10,2), range(0,10,2))   : a 2d slice viewing every each elements with even coordinates.
``
 * matrix, matrix_view when sliced, return vector_view or matrix_view.

* One can be much more general : e.g. slicing the diagonal of a matrix, etc...
  


* Implementation is entirely done in the IndexMaps classes, by specializing 2 templates 
 (basically by writing the function IM1-> IM2).
 The rest is done by indexmap_storage_pair class, which will compute the correct view class
 depending on the view class and IM2 (view_from_tag_I_S template).

::
  //In namespace IndexMaps::result_of
  template<typename IM, typename ArgsTuple> 
  struct slice< IM,  ArgsTuple> { typedef IM2 type; };

  //In namespace IndexMaps : 
  template<typename IM, typename ArgsTuple> 
  typename result_of::slice<IM,ArgsTuple>::type slice(IM const &, ArgsTuple args);

