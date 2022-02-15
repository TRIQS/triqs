**[triqs/stat]** Statistical Analysis
=====================================

Introduction
------------

This library provides statistical tools to analyze samples that are generated during a simulation. In particular, it
provides routines for:

(a) accumulating and binning correlated measurements, arising from a Monte Carlo simulation
(b) calculating averages and errors for data and functions of data
(c) constructing histograms

Processing Correlated Data 
---------------------------

In general, the output samples produced in Monte Carlo type simulations are *not* independent of one another, but
correlated over a characteristic number of steps -- the correlation time :math:`\tau`. This depends on the observable as
well as on the details of the Monte Carlo simulation.  In order to proceed with a conventional statistical analysis, we
have to obtain independent samples from by binning the raw samples. The central object in the code for this is the
:ref:`accumulator class <triqs__stat__accumulator>`. This provides functions to both to estimate :math:`\tau` and to
actually bin and store data.

Averages and Standard Errors 
-----------------------------

For independent samples :math:`\lbrace x_i\rbrace _{i=0\dots N-1}` of the elementary observable :math:`X`, estimates of
the mean and standard error are:

.. math:: \bar{X} \approx \frac{1}{N} \sum_{i=0}^{N-1} x_{i} \\  (\Delta \bar{X})^2 \approx \frac{N-1}{N} \frac{ \sigma^2(x)}{N}

Functions to perform these calculations (including mpi versions), are :ref:`implemented in the library <triqs__stat>`.

We often want to computing the mean and error for quantities which are derived from our measured and binned data.
Consider a general function :math:`f` that depend on several observables :math:`X,Y,\ldots`:

.. math:: f(\bar{X}, \bar{Y}, \dots)

Unless :math:`f` is linear, calculating a reliable estimate the error :math:`\Delta f` is a difficult problem, which can
be addressed using resampling methods -- the simplest of these is the jackknife. We implement the :ref:`jacknife
function <jackknife>` (and a :ref:`MPI version <jackknife_mpi>`), which allows for general user-specified functions
:math:`f` and input data :math:`X,Y,\ldots`.

:ref: `C++ API Reference: </documentation/cpp_api/triqs/stat>`
