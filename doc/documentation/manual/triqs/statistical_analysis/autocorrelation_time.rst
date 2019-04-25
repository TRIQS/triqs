Autocorrelation time 
=======================

The autocorrelation time of a series of samples can be approximated, in the large :math:`N` limit, by :

.. math::

   \tau_b = \frac{1}{2}\left(\frac{b \tilde{\sigma}_b^2}{\mathrm{Var}X}- 1\right)


where :math:`b` is bin size, :math:`\tilde{\sigma}_b^2` the variance of the binned series, :math:`\mathrm{Var}X` is the variance of the random variable :math:`X`, which is well approximated by the variance of the series :math:`\sigma^2`, so that the autocorrelation time may be computed as:


.. math::

   \bar{\tau} = \sum_{b>b_c} \frac{1}{2}\left(\frac{b \tilde{\sigma}_b^2}{\sigma^2} - 1\right)


where :math:`b_c`  is chosen large enough so that :math:`\tau_b` is constant as a function of :math:`b`.

Synopsis
---------

`autocorrelation_time_from_binning(T observable)` 
 - observable: object with **Observable** concept

 returns the autocorrelation time.

`autocorrelation_time(T observable)` 
 - observable: object with **Observable** concept

 returns the autocorrelation time computed from the autocorrelation function (slow).

Example
--------

.. literalinclude:: ./autocorrelation_time_0.cpp
