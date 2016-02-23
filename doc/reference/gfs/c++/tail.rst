.. highlight:: c

.. _gf_tail:

High frequency tail
===========================

Implementation
--------------

In TRIQS, the tail is implemented as an object :ref:`tail`.

A simple example
~~~~~~~~~~~~~~~~~~~~

.. triqs_example:: ./tail_0.cpp

.. toctree::

   triqs/gfs/tail_impl


Fitting the tail of a Green's function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given an imaginary-frequency Green's function, one can compute the moments of its high-frequency tail with the function ``fit_tail``:


.. triqs_example:: ./tail_1.cpp
The full documentation of ``fit_tail`` is :doc:`here<fit_tail>`.
