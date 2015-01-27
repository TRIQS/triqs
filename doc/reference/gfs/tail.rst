.. highlight:: c

.. _gf_tail:

High-frequency tail
===========================

The high-frequency behavior (or **tail**) of a Green's function is of paramount importance in the context of Fourier transformations: since fermionic Green's functions are slowly decaying functions of frequency, special care has to be taken when Fourier-transforming them to time.

Mathematical definition 
---------------------------

The "tail" of a Green's function encodes the behavior of the Green's
function :math:`G` at large Matsubara frequencies, namely

.. math::  \mathbf{G}(i\omega_n) \stackrel {=}{\infty} \mathbf{a}_{-1}\cdot i\omega_n + \mathbf{a}_{0} +\mathbf{a}_{1}\cdot \frac{1}{ i\omega_n} +\mathbf{a}_{2}\cdot \frac{1}{ (i\omega_n)^2} +\dots  


Generically, the tail is parametrized by matrix-valued coefficients
:math:`\mathbf{a}_{i}` (of size :math:`N_1\times N_2`\ )

.. math:: t = \sum_{i=o_{min}}^{o_{max}} \mathbf{a}_i (i\omega_n)^{-i}


Python interface
-------------------

The tail is implemented as a Python object ``TailGf`` with the following constructor:

``TailGf(int N1, int N2, int n_order=10, int order_min=-1)`` where

+-------------------+-----------------------------------------------------------------------------+
| Name              | Description                                                                 |
+===================+=============================================================================+
| ``N1`` and ``N2`` | size of each tail coefficient :math:`\mathbf{a}_{i}`: :math:`N_1\times N_2` |
+-------------------+-----------------------------------------------------------------------------+
| ``n_order``       | number of tail coefficients                                                 |
+-------------------+-----------------------------------------------------------------------------+
| ``order_min``     | smallest known order                                                        |
+-------------------+-----------------------------------------------------------------------------+


It has the following members:

+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Member name                         | Description                                                                                                                                                                                                       |
+=====================================+===================================================================================================================================================================================================================+
| ``data``                            | numpy array representing :math:`\mathbf{a}_{i}` : ``data[i,m,n]`` :math:`= (\mathbf{a}_i)_{m,n}`\ .                                                                                                               |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| ``mask``                            | :math:`N_1\times N_2` numpy array of the maximal index :math:`i_{nm}` of the known coefficients (``order_max`` may be larger than ``mask``, but all coefficients of indices greater than ``mask`` are irrelevant) |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| ``N1`` and ``N2``                   | size of each tail coefficient :math:`\mathbf{a}_{i}`: :math:`N_1\times N_2`                                                                                                                                       |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| ``size``                            | number of coefficients of the tail.                                                                                                                                                                               |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| ``__getitem__`` and ``__setitem__`` | access and set the `i`-th coefficient :math:`\mathbf{a}_{i}` with the bracket operator                                                                                                                            |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| ``__call__``                        | evaluate the tail at a given frequency                                                                                                                                                                            |
+-------------------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+



Initializing a tail object
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The following code snippet illustrates how to instantiate a TailGf object.

.. runblock:: python

    from pytriqs.gf.local import *

    t = TailGf(N1=1, N2=1)
    print "t = ",t
    print "t.data.shape = ",t.data.shape
    print "t.order_min = ",t.order_min
    print "t.order_max = ",t.order_max
    print "t.mask = ",t.mask
    print "t[1] = ",t[1]
    print "t(100) = ",t(100)


As a member of a Green's function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The following code snippet show how to access the tail of a Green's function

.. runblock:: python

    from pytriqs.gf.local import *
    
    # Create the Matsubara-frequency Green's function and initialize it
    g = GfImFreq(indices = [1], beta = 50, n_points = 1000, name = "imp")
    g << inverse( iOmega_n + 0.5 )
    
    print "g.tail = ", g.tail
    print "g.tail[2] = ",g.tail[2]


Fitting the tail
~~~~~~~~~~~~~~~~~~~~

Green's functions have a method ``fit_tail`` allowing to fit the data
contained in the Green's function. ``fit_tail`` is called in the
following way:

``fit_tail(fixed_coeff, order_max, fit_start, fit_stop)`` where

-  ``fixed_coeff`` is a TailGf object (of size :math:`n\times N_1 \times N_2`) containing the
   known coefficients
   (``fixed_coeff[i,n,m]``:math:`\equiv (\mathbf{a}_{-1+i})_{nm}`\ )
-  ``order_max`` is the maximal index of the coefficients to be
   determined
-  ``fit_start`` and ``fit_stop`` are the *indices* of the Matsubara frequencies between which to
   fit the data

In the following example, the Green's function ``g`` defined above is
fitted between :math:`\omega_{10}` and :math:`\omega_{20}` with fixed
coefficients :math:`(\mathbf{a}_{-1})_{00} = 0`\ ,
:math:`(\mathbf{a}_{0})_{00} = 0` and :math:`(\mathbf{a}_{1})_{00} = 1`

.. runblock:: python

    from pytriqs.gf.local import *
    from numpy import array

    g = GfImFreq(indices = [1], beta = 50, n_points = 1000, name = "imp")
    g << inverse( iOmega_n + 0.5 )
    g.tail.zero()
    fixed_coeff = TailGf(1,1,3,-1)
    fixed_coeff[-1] = array([[0.]])
    fixed_coeff[0] = array([[0.]])
    fixed_coeff[1] = array([[1.]])
    fit_n_moments = 4
    fit_start = 10
    fit_stop = 20
    g.fit_tail(fixed_coeff, fit_n_moments, fit_start, fit_stop)
    print "g.tail = ", g.tail


C++ Implementation
--------------------

In c++, the tail is implemented as an object ``tail``.


A simple example
~~~~~~~~~~~~~~~~~~~~

.. triqs_example:: ./tail_0.cpp


API
~~~~~~~~~~
Here are the main methods of the ``tail`` class:

+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| Member                          | Description                                                                             | Type                     |
+=================================+=========================================================================================+==========================+
| data()                          | 3-dim array of the coefficients: ``data(i,n,m)`` :math:`=(\mathbf{a}_{i+o_{min}})_{nm}` | data_view_type           |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| mask_view()                     | 2-dim (:math:`N_1 \times N_2`) array of the maximum non-zero indices                    | mask_view_type           |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| order_min()                     | minimum order                                                                           | long                     |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| order_max()                     | maximum order                                                                           | long                     |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| size()                          | first dim of data()                                                                     | size_t                   |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| shape()                         | shape of data()                                                                         | shape_type               |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| smallest_nonzeros()             | order of the smallest_nonzero coefficient                                               | long                     |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| is_decreasing_at_infinity()     | true if the tail is decreasing at infinity                                              | bool                     |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| operator() (int n)              | matrix_valued coefficient :math:`(\mathbf{a}_i)_{nm}`                                   | mv_type                  |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| get_or_zero (int n)             | matrix_valued coefficient :math:`(\mathbf{a}_i)_{nm}`                                   | const_mv_type            |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
| evaluate(dcomplex const &omega) | value of the tail at frequency omega                                                    | arrays::matrix<dcomplex> |
+---------------------------------+-----------------------------------------------------------------------------------------+--------------------------+
    
   
The tail is DefaultConstructible, H5Serializable and BoostSerializable.


Fitting the tail of a Green's function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given an imaginary-frequency Green's function, one can compute the moments of its high-frequency tail with the function ``fit_tail``:


.. triqs_example:: ./tail_1.cpp
The full documentation of ``fit_tail`` is :doc:`here<fit_tail>`.

