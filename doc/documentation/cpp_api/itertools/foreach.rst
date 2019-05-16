..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _itertools__foreach:

itertools::foreach
==================

*#include <itertools.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename F>`
       | void :red:`foreach` (:ref:`range <itertools__range>` const & :param:`r`, F && :param:`f`)





Apply a function f to every element of an integer range





Parameters
^^^^^^^^^^

 * :param:`r` The range to apply the function to

 * :param:`f` The function to apply