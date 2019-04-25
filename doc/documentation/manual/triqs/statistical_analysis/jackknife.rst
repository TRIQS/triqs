.. highlight:: c

Jackknife
============

The jaccknife method aims at computing an unbiased estimate of the error bar on an observable :math:`f(\langle X \rangle, \langle Y \rangle)` when :math:`f` is nonlinear in its variables.

The jackknifed series is defined from the original series :math:`\lbrace x_i \rbrace _{i=1\dots N}` as:

.. math::

   x_i^J = \frac{1}{N} \sum_{j=1,j\neq i}^{N} x_j


The error estimate is then:

.. math::

   \Delta f(\langle X\rangle) = \sqrt{(N-1) \sigma_{f^J}^2}


where :math:`f^J_i = f(x_i^J)` and :math:`\sigma_{f^J}^2` is the variance of this series.


Synopsis
---------


`make_jackknife(T series)` 
 - series: object with **TimeSeries** concept

 returns the jackknifed time series.





Example
---------

.. literalinclude:: ./jackknife_0.cpp
