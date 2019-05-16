..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__hilbert_space__imperative_operator__update_coeffs:

triqs::hilbert_space::imperative_operator::update_coeffs
========================================================

*#include <triqs/hilbert_space/imperative_operator.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename Lambda>`
       | void :red:`update_coeffs` (Lambda :param:`L`)





Apply a callable object to each coefficient of the operator by reference

  The callable object must take one argument convertible to `ScalarType &`





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`Lambda` Type of the callable object


Parameters
^^^^^^^^^^

 * :param:`L` Callable object