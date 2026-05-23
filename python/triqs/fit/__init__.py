# Copyright (c) 2013 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020 Simons Foundation
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
# Authors: Olivier Parcollet, Nils Wentzell


r"""
Lightweight least-squares curve fitting for 1D data.

This module wraps :func:`scipy.optimize.leastsq` in a small,
plotting-friendly :class:`~triqs.fit.fit.Fit` class. A fit is
performed at construction time, the resulting object is callable
(``fit(x)`` returns the fitted model evaluated at ``x``) and
stringifies to a LaTeX expression -- so it slots directly into
:func:`triqs.plot.mpl_interface.oplot`.

Two ready-made model definitions are provided as convenience
``fitter`` tuples: :data:`~triqs.fit.fit.linear` and
:data:`~triqs.fit.fit.quadratic`.

It is imported with the command::

  >>> from triqs.fit import Fit, linear, quadratic
"""

from .fit import Fit, linear, quadratic

__all__ = ['Fit', 'linear', 'quadratic']
