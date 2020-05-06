.. index:: Random number generator

.. module:: triqs.random_generator

.. _random_generator:

Random number generators
==========================

TRIQS exposes to python the random number generators used in C++, 
in the module ``RandomGenerator``. 
The generators are the boost random generators.

Usage
-----

The generator is constructed from a name (the name of the boost generator) and a seed::

  from triqs.random_generator import *
  r = RandomGenerator("mt19937", 237849)

A list of available random generators is obtained with::

  print random_generator_names_list()

Then you can either generate float number on the interval :math:`[0,1[` 
simply by calling the generator, or integer numbers in the inverval :math:`[0,N-1]` by calling 
it with `N` ::

  print r()
  print r(10)

Example
-------

Here's a simple example showing how to use the generator.

.. plot:: documentation/manual/triqs/random_generator/random.py
   :include-source:
   :scale: 70


Complete documentation/manual/triqs
------------------

.. autoclass:: triqs.random_generator.RandomGenerator
  :members:

.. autofunction:: triqs.random_generator.random_generator_names_list
