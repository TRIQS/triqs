.. highlight:: c

.. _util_tuple:

Tuple compile time tools
=============================

Very useful for lib developers, they fill a missing gap in the std library.
They implement various standard functional operations, at compile time, 
on tuple...

.. note::

   Simple measures have shown that these routines are **as fast as native code** (tested on gcc, clang, icc),
   due to inlining. They can therefore be used in critical parts of codes.

apply
-----------------------------------------------

*Purpose* : `apply a function on a tuple of arguments`

 Given a function object `f`, and its arguments stored in a tuple `t`, and we want to apply `f` on `t`.
 
 Python equivalent : `f(*t)`

*Synopsis* ::

  template<typename Function, typename Tuple> auto apply (Function && f, Tuple const & t);

*Solution* :

.. triqs_example:: ./tupletools_0.cpp
for_each 
-------------------------------------------------------------------------

*Purpose* : `apply a function for each element of a tuple (in order)`


 Given a function object `f`, we want to apply it to all elements of a tuple `t`.

 Python equivalent : `for x in t : f(x)`

*Synopsis* ::

  template<typename Function, typename Tuple> void for_each(Tuple const & t, Function && f);
 
*Solution* :

.. triqs_example:: ./tupletools_1.cpp
