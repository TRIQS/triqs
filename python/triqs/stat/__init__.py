# Copyright (c) 2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2016 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020-2023 Simons Foundation
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You may obtain a copy of the License at
#     https:#www.gnu.org/licenses/gpl-3.0.txt
#
# Authors: Alexander Hampel, Priyanka Seth, Nils Wentzell


r"""
Statistical analysis helpers from the TRIQS stat framework.

This module exposes the histogram class used to sample and bin a
continuous random variable, together with the helpers that turn a
histogram into a probability density function (:func:`pdf`) or a
cumulative distribution function (:func:`cdf`).

Other statistical utilities living in the C++ ``triqs::stat``
namespace -- linear/logarithmic binning accumulators, jackknife
resampling, mean/error analysis -- are not currently exposed to
Python.

It is imported with the command::

  >>> from triqs.stat import Histogram, cdf, pdf
"""
from .histograms import Histogram, cdf, pdf

# register the plot function in the plotting table
from .histogram import plot
from triqs.plot.protocol import plot_function_table
plot_function_table[Histogram] = plot

__all__ = ['Histogram', 'cdf', 'pdf']
