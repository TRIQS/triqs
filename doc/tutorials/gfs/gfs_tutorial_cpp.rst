Green's functions manipulations in C++
=======================================

.. highlight:: c

.. toctree::
   :maxdepth: 1

The gfs class of TRIQS contains objects representing Green functions over real or imaginary times, real or imaginary frequencies...  that can be easily manipulated
(see  :doc:`here <../../reference/gfs/c++/contents>`).
Here are a couple of simple examples showing the basic use of this class.

Creation of a real time GF
-----------------------------

Here we create a GF defined on the time interval from tmin to tmax.
If we want the value of the GF at any time to be a scalar, we use:

.. triqs_example:: ./gfs_tutorial_0.cpp
If we need a matrix of size n by m, we use:

.. triqs_example:: ./gfs_tutorial_1.cpp
Or a tensor!

.. triqs_example:: ./gfs_tutorial_2.cpp
Creation of other simple GF's
-------------------------------

.. triqs_example:: ./gfs_tutorial_3.cpp
Creation of a two real time GF
--------------------------------

.. triqs_example:: ./gfs_tutorial_4.cpp
How to fill a GF with placeholders
-----------------------------------

.. triqs_example:: ./gfs_tutorial_5.cpp
How to interpolate the GF value at a point of the domain
---------------------------------------------------------

You simply have to call the GF with the coordinates of the point: 

.. triqs_example:: ./gfs_tutorial_6.cpp
Learn more in the full reference, see :ref:`greenfunctions`


