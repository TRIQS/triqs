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
Least-squares fit of a parametric model to 1D data.

Provides :class:`Fit`, a thin wrapper around
:func:`scipy.optimize.leastsq`, together with two ready-made model
tuples :data:`linear` and :data:`quadratic` that can be passed
directly as the ``fitter`` argument.
"""

from scipy.optimize import leastsq
import numpy as np, inspect as ins

class Fit:
    r"""
    Least-squares fit of a parametric model to 1D data.

    Given a model ``f(x, p_0, ..., p_{n-1})`` and an initial guess,
    :class:`Fit` adjusts the parameters by minimising
    :math:`\sum_i |f(x_i, \mathbf{p}) - y_i|^2` using
    :func:`scipy.optimize.leastsq`. The fit is performed in
    ``__init__``; the constructed object is callable and stringifies
    to a LaTeX expression, so it can be passed directly to
    :func:`triqs.plot.mpl_interface.oplot`.

    Parameters
    ----------
    x_array : array-like
        1D array of abscissae.
    y_array : array-like
        1D array of ordinates, same length as ``x_array``.
    fitter : tuple
        Triple ``(F, name, init_value_default)`` where ``F`` is a
        callable ``F(x, *params) -> y`` acting elementwise on a numpy
        array, ``name`` is a printf-style template such that
        ``name % params`` yields a LaTeX expression, and
        ``init_value_default`` is a tuple of default initial values
        used when ``p0`` is ``None``. Ready-made fitters:
        :data:`linear`, :data:`quadratic`.
    p0 : tuple, optional
        Initial guess. Defaults to the third element of ``fitter``.

    Attributes
    ----------
    function : callable
        The model ``F`` extracted from ``fitter``.
    fname : str
        The printf-style LaTeX template extracted from ``fitter``.
    param : numpy.ndarray
        Fitted parameter values returned by ``scipy.optimize.leastsq``.

    Examples
    --------
    Linear fit of noisy data, then plotted on top of the data via
    :func:`~triqs.plot.mpl_interface.oplot`::

        from triqs.fit import Fit, linear
        from triqs.plot.mpl_interface import oplot, plt
        f = Fit(x, y, linear)
        oplot(f)
        plt.show()
    """
    def __init__ (self, x_array, y_array, fitter, p0 = None ) :
        self.function, self.fname, p00 = fitter
        assert len(ins.getfullargspec(self.function)[0])== len(p00) + 1, "error in number of parameters"
        assert len(y_array) == len(x_array)
        assert len(y_array) > len (p00)
        errfunc = lambda x :  np.abs ( self.function(x_array,*x)  - y_array[:])
        self.param, success = leastsq(errfunc, p0 if p0 else p00 )

    def __str__ (self) :
        r"""LaTeX representation ``fname % param`` (with ``+ -`` collapsed to ``-``)."""
        return (self.fname%tuple(self.param) or 'Fit').replace("+ -","-")
    def __repr__ (self) : return str(self)
    def __repr_tex__ (self) : return str(self)
    def __call__ (self,x) :
        r"""
        Evaluate the fitted model at ``x``.

        Parameters
        ----------
        x : array_like or float
            Point(s) at which to evaluate the model.

        Returns
        -------
        ndarray or float
            ``function(x, *param)``.
        """
        return self.function(x,*self.param)


# a collection of useful fit ...

linear             =  lambda X, a,b   : a * X + b,             r"$%f x + %f$"          , (1,1)
r"""Linear-model ``fitter`` :math:`y = a\,x + b`.

Triple ``(F, name, init_value_default)`` consumable by :class:`Fit`:

* ``F`` -- ``lambda X, a, b: a * X + b``
* ``name`` -- ``r"$%f x + %f$"``
* ``init_value_default`` -- ``(1, 1)``
"""

quadratic          =  lambda X, a,b,c : (a * X + b)*X + c,     r"$%f x^2 + %f x + %f$" , (0,1,1)
r"""Quadratic-model ``fitter`` :math:`y = a\,x^2 + b\,x + c`.

Triple ``(F, name, init_value_default)`` consumable by :class:`Fit`:

* ``F`` -- ``lambda X, a, b, c: (a * X + b) * X + c``
* ``name`` -- ``r"$%f x^2 + %f x + %f$"``
* ``init_value_default`` -- ``(0, 1, 1)``
"""


