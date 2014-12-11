Binning
==========

Binning (or *bunching*) consists in grouping the terms of a statistical series in order to reduce its autocorrelation time. If the bin size is large enough, the samples of the binned series are uncorrelated; as a result, its error bar can be computed easily, i.e

.. math::
     \Delta \langle X \rangle = \sqrt{\frac{\tilde{\sigma}^2} {\tilde{N}-1}}


where :math:`\tilde{\sigma}^2` and :math:`\tilde{N}` are respectively the variance and size of the binned series.

Synopsis
----------
 
`make_binned_series(T series, int bin_size)` 
 - series: object with **TimeSeries** concept
 - bin_size: size of the bin

 returns the binned time series.

Example
--------

.. triqs_example:: ./binning_0.cpp
