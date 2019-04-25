Autocorrelation function
=======================

The autocorrelation function of a series of samples is defined as:

.. math::

   A(k) =\frac{\langle x_{i+k} x_i \rangle - \langle x \rangle ^2}{ \langle x^2 \rangle - \langle x \rangle ^2} \approx\frac{1}{\sigma^2(N-k)}\sum_{i=0}^{N-k-1} (x_{i+k}-\overline{x})(x_i-\overline{x})


where :math:`\overline{x}` is the mean, :math:`\sigma^2` the variance of the series.


Synopsis
---------

`make_normalized_autocorrelation(T observable)` 
 - observable: object with **Observable** concept

 returns the autocorrelation function as a TimeSeries.


Example
--------

.. literalinclude:: ./autocorrelation_function_0.cpp
