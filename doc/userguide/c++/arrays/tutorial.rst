Multidimensional arrays
=======================

.. highlight:: c

.. toctree::
   :maxdepth: 1

TRIQS comes with a library of multidimensional arrays. 
This library, among others, allows for easy slicing, archiving and algebraic manipulations of multidimensional arrays. 
Here are a couple of simple examples showing the basic use of this class.


Declaring and printing an array
-------------------------------
.. literalinclude:: tutorial_0.cpp
   :language: cpp

Simple operations
-------------------
.. literalinclude:: tutorial_1.cpp
   :language: cpp

HDF5 Archiving
-------------------
Archiving an array into an HDF5 file is easy:

.. literalinclude:: tutorial_2.cpp
   :language: cpp

Views: ranges and slices
-------------------------
One can easily take a slice of an array to view and modify only part of the underlying data.

.. literalinclude:: tutorial_3.cpp
   :language: cpp

Matrices and vectors
-------------------------
Arrays must be distinguished from vectors and matrices, which have an algebra of their own.

.. literalinclude:: tutorial_4.cpp
   :language: cpp

Defining through a lazy expression
-----------------------------------
.. literalinclude:: tutorial_5.cpp
   :language: cpp

Linear algebra
---------------
.. literalinclude:: tutorial_6.cpp
   :language: cpp

Map and fold
-------------
.. literalinclude:: tutorial_7.cpp
   :language: cpp

The full reference of the array library can be found :doc:`here <../../reference/arrays/contents>`.
