# Copyright (c) 2013 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013 Centre national de la recherche scientifique (CNRS)
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
# Authors: Michel Ferrero, Olivier Parcollet


r"""
Matplotlib helpers and a generic plot protocol for TRIQS objects.

This package is organised in two layers:

* :mod:`triqs.plot.protocol` defines the *plot protocol*, a uniform
  way for TRIQS objects to describe themselves to a plotting backend.
  An object participates by either implementing ``_plot_(opt_dict)``
  or by registering a free function in
  :data:`triqs.plot.protocol.plot_function_table`.
* :mod:`triqs.plot.mpl_interface` provides :func:`~triqs.plot.mpl_interface.oplot`,
  a thin matplotlib wrapper that consumes the plot protocol so that
  Green's functions, histograms, lattice objects and plain arrays can
  all be passed to the same plotting call.

Typical usage::

  >>> from triqs.plot.mpl_interface import oplot, plt
  >>> oplot(g, mode='R')
  >>> plt.show()
"""

__all__ = []
