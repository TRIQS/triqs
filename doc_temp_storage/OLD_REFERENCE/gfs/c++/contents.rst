.. _greenfunctions:

C++ documentation
=========================

The TRIQS library provides a generic container `gf` and its views `gf_view` and `gf_const_view`, to store and manipulate
various Green functions on various meshes, e.g.: 

* Matsubara frequency(ies),
* Matsubara time(s),
* real time(s),
* real frequency(ies),
* Legendre representation.
* Block of other green functions.
* Cartesian product of green functions.

Various free functions are also provided to handle transformations between various representation of a Green function, 
such as Fourier transforms.


.. toctree::
   :maxdepth: 2

   auto_ref
   gf_and_view
   gf_special
   clef
   fourier
   tail
   concepts
   implementation_notes
