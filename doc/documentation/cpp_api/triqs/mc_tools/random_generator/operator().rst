..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__mc_tools__random_generator__operator():

triqs::mc_tools::random_generator::operator()
=============================================

*#include <triqs/mc_tools/random_generator.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :green:`template<typename T>`
       | typename std::enable_if<std::is_integral<T>::value, T>::type :red:`operator()` (T :param:`i`)

    2. | double :red:`operator()` (double :param:`x`)

    3. | double :red:`operator()` (double :param:`a`, double :param:`b`)

Documentation



 **1)**   Returns a integer in [0,i-1] with flat distribution



 **2)**   Returns a double in [0,x[ with flat distribution



 **3)**   Returns a double in [a,b[ with flat distribution