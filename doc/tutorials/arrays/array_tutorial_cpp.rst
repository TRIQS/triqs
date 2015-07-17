Tutorial on multidimensional arrays in C++
============================================

.. highlight:: c

.. toctree::
   :maxdepth: 1

TRIQS comes with a library of multidimensional arrays. 
This library, among others, allows for easy slicing, archiving and algebraic manipulations of multidimensional arrays. 
Here are a couple of simple examples showing the basic use of this class.


Declaring and printing an array
-------------------------------
.. triqs_example:: ./array_tutorial_0.cpp

Simple operations
-------------------
.. triqs_example:: ./array_tutorial_1.cpp

HDF5 Archiving
-------------------
Archiving an array into an HDF5 file is easy:
.. triqs_example:: ./array_tutorial_2.cpp

Views: ranges and slices
-------------------------
One can easily take a slice of an array to view and modify only part of the underlying data.
.. triqs_example:: ./array_tutorial_3.cpp

Matrices and vectors
-------------------------
Arrays must be distinguished from vectors and matrices, which have an algebra of their own.
.. triqs_example:: ./array_tutorial_4.cpp

Defining through a lazy expression
-----------------------------------
.. triqs_example:: ./array_tutorial_5.cpp

Linear algebra
---------------
.. triqs_example:: ./array_tutorial_6.cpp

Map and fold
-------------
.. triqs_example:: ./array_tutorial_7.cpp
The full reference of the array library can be found :doc:`here <../reference/arrays/contents>`.
