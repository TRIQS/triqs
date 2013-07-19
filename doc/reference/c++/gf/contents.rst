.. _greenfunctions:

The Green function class
========================

The TRIQS library has a class called ``gf`` which allows you to use easily a whole set of Green functions. 

You can use as variable(s)

* real time(s),

* real frequency(ies),

* Matsubara time(s),

* Matsubara frequency(ies),

* or other variables that you can define.

More generally, the variable is a point of a ``domain``

The value of the Green function on a point of the domain can be a scalar, a matrix or whatever you want (this type is called type ``target_t``). 

You can group several green functions into *blocks* (for example, one block per orbital, or per wave vector...). 

Fourier transforms are implemented for these Green functions:

    real time <-> real frequency

    Matsubara time <-> Matsubara frequency


.. toctree::
   :maxdepth: 2

   concepts
   meshes
   the_four_basic_GFs
   fourier
   cookbook/contents
