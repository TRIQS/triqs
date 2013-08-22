.. highlight:: c

.. _intro:

Introduction
=================

The TRIQS library provides a generic container `gf` and its view `gf_view`, to store and manipulate
various Green functions.

Several specific green functions are implemented : 

* real time(s),
* real frequency(ies),
* Matsubara time(s),
* Matsubara frequency(ies),
* Legendre representation.
* Block of other green functions.
* Cartesian product of green functions.

Various free functions are also provided to handle transformations between various representation of a Green function, 
such as Fourier transforms.

More generally, the variable is a point of a ``domain``

The value of the Green function on a point of the domain can be a scalar, a matrix or whatever you want (this type is called type ``target_t``). 

You can group several green functions into *blocks* (for example, one block per orbital, or per wave vector...). 

Fourier transforms are implemented for these Green functions:

    real time <-> real frequency

    Matsubara time <-> Matsubara frequency

    
This section is organized as follows : 

* the concepts related to Green functions, their domain, their representation on a grid.
* the container `gf` and its view `gf_view`, which represent various Green functions.



