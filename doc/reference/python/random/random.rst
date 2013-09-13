.. index:: Random number generator

.. module:: pytriqs.random

.. _random_generator:

Random generator class
======================

TRIQS has a simple random number generator class called ``RandomGenerator``. It is based on boost
just like the C++ random generator provided by TRIQS.

Usage
-----

The generator is constructed from a name and a seed::

  from pytriqs.random import *
  r = RandomGenerator("mt19937", 237849)

A list of possible random generator names is obtained with::

  print available_generator_names()

Then you can either generate float number on the interval :math:`[0,1[` using
the ``rand()`` method or integer numbers in the inverval :math:`[0,N-1]` using
``int_rand(N)``::

  print r.rand()
  print r.int_rand(10)

Example
-------

Here's a simple example showing how to use the generator.

.. plot:: reference/python/random/random.py
   :include-source:
   :scale: 70


Complete reference
------------------

.. autoclass:: pytriqs.random.RandomGenerator
  :members:

.. autofunction:: pytriqs.random.available_generator_names
