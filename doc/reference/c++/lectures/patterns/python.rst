Python bindings
#####################

All the idioms that we use from boost.python ....

Introduction to boost.python
==============================

* wrapping versus converting.

Example of wrapping a function, a class, with an array.

Using a more complex converters.

Wrapping enable a converter.

* converting : principle.
  Ref to doc.
  The simple triqs way to build a converter.
  Example of range !

* Passing a pyobject. 

* Difference between PyObject* and bpy::object
  Reference counting.

* More sophisticated data structure.

* THE TRIQS tools : a thin layer (converter) for recursive conversion
  (e.g. vector<T> , unordered_map, ....)


boost.python : a C++ API for python
======================================

Code injection
=================
 
The problem.

The solution : injection and documentation.

Examples
==========

Passing a dict for parameters in a routine.

Passing a list of gf_view into a vector of gf_view...
---> make it a const vector to avoid changing it....

Other options
==============

* Swig
* Cython


