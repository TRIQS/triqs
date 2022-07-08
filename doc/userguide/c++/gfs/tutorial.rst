Green's functions
=================

.. highlight:: c

.. toctree::
   :maxdepth: 1

The `gfs` class of TRIQS contains objects representing Green functions over real or imaginary times, real or imaginary frequencies...  that can be easily manipulated at the C++ level.
Here are a couple of simple examples showing the basic use of this class. Learn more in the  :doc:`full reference <../../reference/gfs/c++/contents>`.


Matsubara Green's functions
-----------------------------

Creation of a simple Green's function :math:`G(i\omega)`
.........................................................
In this example, we show how to initialize the following Green's functions:

.. math::
    G(i\omega) = \frac{1}{i\omega -3}


.. literalinclude:: tut_matsub_1.cpp
    :language: cpp

Two-frequency Green's function :math:`G(i\omega,i\nu)`
........................................................
In this example, we show how to initialize the following Green's functions:

.. math::
    G(i\omega, i\nu) = \frac{1}{i\omega + i\nu -4}


.. literalinclude:: tut_matsub_2.cpp
    :language: cpp

Imaginary-time Green's functions :math:`G(\tau)`
--------------------------------------------------

.. literalinclude:: tutorial_3.cpp
    :language: cpp

Real-time Green's functions :math:`G(t)`
----------------------------------------------

Here we create a GF defined on the time interval from tmin to tmax.
If we want the value of the GF at any time to be a scalar, we use:

.. literalinclude:: tutorial_0.cpp
    :language: cpp

If we need a matrix of size n by m, we use:

.. literalinclude:: tutorial_1.cpp
    :language: cpp

Or a tensor!

.. literalinclude:: tutorial_2.cpp
    :language: cpp

Creation of a two real time GF :math:`G(t,t')`
................................................

.. literalinclude:: tutorial_4.cpp
    :language: cpp

How to fill a GF with placeholders
...................................

.. literalinclude:: tutorial_5.cpp
    :language: cpp

How to interpolate the GF value at a point of the domain 
.............................................................................

You simply have to call the GF with the coordinates of the point: 

.. literalinclude:: tutorial_6.cpp
    :language: cpp

Learn more in the full reference, see :ref:`greenfunctions`
