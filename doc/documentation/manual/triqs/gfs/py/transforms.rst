.. index::
  single: Transforming Green's functions


Fourier and Legendre to Matsubara transforms
============================================

Here are the different ways to transform Green's functions of one type into a different type.

Time to frequency
-----------------

The command ``Fourier`` allows you to initialize a block
Green's function defined in frequency (time) from a block Green's function defined
in time (frequency). Here's a simple example:

.. plot:: documentation/manual/triqs/gfs/py/fourier.py
   :include-source:
   :scale: 70

The same command can be used for real-time <-> real-frequency transformations.

Legendre to Matsubara
---------------------

The commands ``MatsubaraToLegendre`` and ``LegendreToMatsubara`` allow to
initialize a Matsubara Green's function (either in imaginary time or in
imaginary frequencies) from a Legendre Green's function and conversely.
Example:

.. plot:: documentation/manual/triqs/gfs/py/legendre.py
   :include-source:
   :scale: 70
