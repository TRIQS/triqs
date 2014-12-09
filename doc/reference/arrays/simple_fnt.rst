.. highlight:: c

.. _arr_simple_fun:

Simple array functions
==================================

The following functions work for any object modeling :ref:`ImmutableCuboidArray` concept.

In this section, we present some simple functions already in the library.
It is however very easy to create new one using the usual functional constructions :ref:`map and fold<arr_map_fold>`.


Element-wise functions
---------------------------

The following functions are mapped on the array (using :ref:`map`):

`abs  real  imag  pow  cos  sin  tan  cosh  sinh  tanh  acos  asin  atan  exp  log  sqrt  floor`
 
meaning that e.g. if `A` is an array,
real(A) models :ref:`ImmutableCuboidArray`, with the same size and returns the real part of the elements.
In other words, it applies the function term-by-term.

.. note:: 

  These functions do NOT compute a new array in memory, they are lazy.


sum 
---------------

**Synopsis**::

  template<ImmutableCuboidArray A> A::value_type sum (A const &);
  template<ImmutableCuboidArray A> A::value_type prod (A const &);

Returns the sum (resp. product) of the all the elements of the array.

max_element, min_element
-------------------------------

**Synopsis**::

  template<ImmutableCuboidArray A> A::value_type max_element (A const &);
  template<ImmutableCuboidArray A> A::value_type min_element (A const &);

Returns the maximum/minimum element of the array (provided that value_type is an ordered type of course...).


Examples
-----------------

.. triqs_example:: ./simple_fnt_0.cpp
