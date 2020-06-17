
.. module:: triqs.fit.fit

Fitting data
====================

A simple example 
^^^^^^^^^^^^^^^^^^^^^

Let us for example fit the Green function :


.. plot:: documentation/manual/triqs/plotting_protocols/fit/fit_test.py
   :include-source:
   :scale: 70

Note that `x_window` appears in the `x_data_view` method to clip the data on a given window
and in the plot function, to clip the plot itself. 

..
  A more complex example
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^
  To illustrate the use of python in a more complex situation, 
  let us demonstrate a simple data analysis.
  This does not use any TRIQS objects, it is just a little exercise in python.
  Imagine that we have 10 Green's function coming from a calculation in a hdf5 file.
  For the need of the demonstration, we will create them "documentation/manually" here, but it is just to make life easier.
  STILL NEEDS TO BE WRITTEN


Reference 
^^^^^^^^^^^^^^^

The Fit class is very simple and is provided for convenience, but the reader
is encouraged to have a look through it and adapt it (it is simply a call to 
scipy.leastsq).

.. autoclass:: triqs.fit.Fit
  :members: 


Example of fitfunc:: 
         
  linear    =  lambda X, a,b  : a * X + b,             r"$%f x + %f$"          , (1,1) 
  quadratic =  lambda X, a,b,c: (a * X + b)*Omega + c, r"$%f x^2 + %f x + %f$" , (0,1,1) 



