**[triqs/statistics]** Tools for statistical analysis: binning, jackknife and autocorrelation time
====================================================================================================

Introduction
------------
Given the statistical samples :math:`\lbrace x_i\rbrace _{i=0\dots N-1}` and   :math:`\lbrace y_i\rbrace _{i=0\dots N-1}` of  random variables :math:`X` and :math:`Y`, one often wants to compute the estimate of the following observables:

:math:`\langle X \rangle`,  :math:`\langle X\rangle/\langle Y \rangle`, :math:`\langle X \rangle^2`, or in general  :math:`f(\langle X \rangle , \langle Y \rangle, \dots)`

as well as the estimate of the errors:

:math:`\Delta\langle X \rangle`,  :math:`\Delta\langle X\rangle /\langle Y \rangle`, :math:`\Delta\langle X\rangle ^2` or :math:`\Delta f(\langle X \rangle , \langle Y \rangle, \dots)`

The estimate of the expectation values is the empirical average :

:math:`\langle X \rangle \approx \frac{1}{N} \sum_{i=0}^{N-1} x_i`

If the samples are independent from each other and :math:`f` is a linear function of its variables (e.g :math:`f=Id`):

:math:`(\Delta \langle X \rangle)^2 \approx \frac{\frac{N-1}{N} \sigma^2({x})}{N}`

where :math:`\sigma^2({x})` is the empirical variance of the sample.


In the general case, however,

  - the samples are correlated (with a characteristic correlation time): one needs to :doc:`bin <binning>`  the series to obtain a reliable estimate of the error bar
  - :math:`f` is non-linear in its arguments: one needs to :doc:`jackknife <jackknife>` the series


This library allows one to reliably compute the estimates of :math:`f(\langle X \rangle , \langle Y \rangle, \dots)` and its error bar :math:`\Delta f(\langle X \rangle , \langle Y \rangle, \dots)` in the general case.

Synopsis
---------
`average_and_error` takes an object with the **Observable** concept (see below) and returns a struct with two members `val` and `error`:
 - `val` is the estimate of the expectation value of the random variable for a given sample of it
 - `error`  is the estimate of the error on this expectation value for the given sample

Concepts
---------

TimeSeries
~~~~~~~~~~
An object has the concept of a TimeSeries if it has the following member functions:

+-------------+-------------------+
| Return type | Name              |
+=============+===================+
| value_type  | operator[](int i) |
+-------------+-------------------+
| int         | size()            |
+-------------+-------------------+

and the following member type:

+-------------+--------------------------------------------+
|  Name       | Property                                   |
+=============+============================================+
|  value_type | belong to an algebra (has +,-,* operators) |
+-------------+--------------------------------------------+

Observable
~~~~~~~~~~

An object has the concept of an observable if it is a TimeSeries and has, additionally, the following member function:

+-------------+-----------------+
| Return type | Name            |
+=============+=================+
| observable& | operator<<(T x) |
+-------------+-----------------+

where `T` belongs to an algebra.

Example
-------

.. triqs_example:: ./contents_0.cpp

.. toctree::
    accumulator
    binning
    jackknife
    autocorrelation_time
    autocorrelation_function
    green_function
    ising2d

Histogram
---------

`histogram` is a lightweight object used to represent and to accumulate a histogram of a real random variable.

.. triqs_example:: ./histogram_example.cpp

.. toctree::
   /cpp2rst_generated/triqs/statistics/histogram
