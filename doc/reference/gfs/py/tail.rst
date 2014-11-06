High-Frequency Tail (``TailGf``)
=========================================

Definition
----------------------

The tail of a Green's function is defined as the behavior of the Green's
function :math:`G` at large Matsubara frequencies, namely

.. math::  \mathbf{G}(i\omega_n) \stackrel {=}{\infty} \mathbf{a}_{-1}\cdot i\omega_n + \mathbf{a}_{0} +\mathbf{a}_{1}\cdot \frac{1}{ i\omega_n} +\mathbf{a}_{2}\cdot \frac{1}{ (i\omega_n)^2} +\dots  

Generically, the tail is parametrized by matrix-valued coefficients
:math:`\mathbf{a}_{i}` (of size :math:`N_1\times N_2`\ )

.. math:: t = \sum_{i=o_{min}}^{o_{max}} \mathbf{a}_i (i\omega_n)^{-i}

Implementation
--------------

In TRIQS, the tail is contained in an Python object ``TailGf`` with the
following members:

-  ``data`` is a numpy array representing :math:`\mathbf{a}_{i}` :
   ``data[i,m,n]`` :math:`= (\mathbf{a}_i)_{m,n}`\ .
-  ``mask`` is the :math:`N_1\times N_2` numpy array of the maximal
   index :math:`i_{nm}` of the known coefficients (``order_max`` may be
   larger than ``mask``, but all coefficients of indices greater than
   ``mask`` are irrelevant)
-  ``N1`` and ``N2`` give the size of each tail coefficient
   :math:`\mathbf{a}_{i}`: :math:`N_1\times N_2`
-  ``size`` is the number of coefficients of the tail.
-  ``__getitem__``and ``__setitem__`` operators: access and set the ith
   coefficient :math:`\mathbf{a}_{i}` with the bracket operator
-  ``__call__`` operator: evaluate the tail at a given frequency

Example
-------

Basic ``TailGf`` object
~~~~~~~~~~~~~~~~~~~~~~~


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

Basic access
^^^^^^^^^^^^


.. runblock:: python

    from pytriqs.gf.local import *
    
    # Create the Matsubara-frequency Green's function and initialize it
    g = GfImFreq(indices = [1], beta = 50, n_points = 1000, name = "imp")
    g <<= inverse( iOmega_n + 0.5 )
    
    print "g.tail = ", g.tail
    print "g.tail[2] = ",g.tail[2]

Fitting tails: ``fit_tail``
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Green's functions have a method ``fit_tail`` allowing to fit the data
contained in the Green's function. ``fit_tail`` is called in the
following way:

``fit_tail(fixed_coeff, order_max, fit_start, fit_stop)`` where

-  ``fixed_coeff`` is the :math:`n\times N_1 \times N_2` numpy array of
   know coefficients
   (``fixed_coeff[i,n,m]``:math:`\equiv (\mathbf{a}_{-1+i})_{nm}`\ )
-  ``order_max`` is the maximal index of the coefficients to be
   determined
-  ``fit_start`` and ``fit_stop`` are the frequencies between which to
   fit the data

In the following example, the Green's function ``g`` defined above is
fitted between :math:`\omega_n=10` and :math:`\omega_n = 20` with fixed
coefficients :math:`(\mathbf{a}_{-1})_{00} = 0`\ ,
:math:`(\mathbf{a}_{0})_{00} = 0` and :math:`(\mathbf{a}_{1})_{00} = 1`

.. runblock:: python

    from pytriqs.gf.local import *
    from numpy import array
    
    g = GfImFreq(indices = [1], beta = 50, n_points = 1000, name = "imp")
    g << inverse( iOmega_n + 0.5 )
    g.tail.zero()
    fixed_coeff = TailGf(1,1,3,0)
    fixed_coeff[0] = array([[0.]])
    fixed_coeff[1] = array([[0.]])
    fixed_coeff[2] = array([[1.]])
    fit_n_moments = 4
    fit_start = 10
    fit_stop = 20
    g.fit_tail(fixed_coeff, fit_n_moments, fit_start, fit_stop)
    print "g.tail = ", g.tail
