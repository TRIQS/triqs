# Copyright (c) 2014-2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2014-2016 Centre national de la recherche scientifique (CNRS)
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
# Authors: Priyanka Seth, Nils Wentzell

r"""
One-dimensional root finding by bracketing and bisection.

A typical TRIQS use case is adjusting the chemical potential
:math:`\mu` so that the particle density reaches a target value.
"""

import scipy.optimize

def determine_bounds(F, x_0, dx, maxiter):
    r"""
    Bracket a sign change of a monotonic real-valued function.

    Starting from ``x_0`` and stepping outward by ``dx`` (the sign of
    the step is chosen from a single forward finite difference so the
    function decreases toward the lower bound and increases toward the
    upper bound), returns an interval :math:`[a, b]` such that
    :math:`F(a) < 0 < F(b)`.

    Parameters
    ----------
    F : callable
        Real-valued function of one real argument. Assumed to be
        monotonic on the relevant range.
    x_0 : float
        Initial guess used as the starting point for the search.
    dx : float
        Magnitude of the step taken at each iteration. The sign is set
        internally from ``F(x_0 + dx) - F(x_0)``.
    maxiter : int
        Maximum number of outward steps allowed in either direction
        before giving up.

    Returns
    -------
    a : float
        Lower bound with ``F(a) < 0``.
    b : float
        Upper bound with ``F(b) > 0``.

    Raises
    ------
    ValueError
        If no lower or upper bound is found within ``maxiter`` steps.
    """
    # Determine if function is increasing or decreasing
    if F(x_0 + dx) - F(x_0) < 0: dx *= -1.0
    # Determine lower bound
    x = x_0; i = 0
    while not F(x) < 0.0:
        x -= dx
        i += 1
        if i > maxiter: raise ValueError("determine_bounds: cannot find lower bound.")
    a = x
    # Determine upper bound
    x = x_0; i = 0
    while not F(x) > 0.0:
        x += dx
        i += 1
        if i > maxiter: raise ValueError("determine_bounds: cannot find upper bound.")
    b = x
    return a, b

def bound_and_bisect(f, x_0, y=0.0, dx=1.0, xtol=1e-3, x_name='x', y_name='y', maxiter=1000, verbosity=1):
    r"""
    Solve :math:`f(x) = y` for a monotonic function ``f``.

    First brackets a sign change of :math:`F(x) \equiv f(x) - y` via
    :func:`determine_bounds`, then refines the root with
    :func:`scipy.optimize.bisect`.

    Parameters
    ----------
    f : callable
        Real-valued, monotonic function of one real argument.
    x_0 : float
        Initial guess used to start the outward bracketing search.
    y : float, optional
        Target value. Default 0.
    dx : float, optional
        Step size used by :func:`determine_bounds`. Default 1.0.
    xtol : float, optional
        Absolute tolerance on ``x`` passed to
        :func:`scipy.optimize.bisect`. Default 1e-3.
    x_name : str, optional
        Display name for the unknown ``x``, used in the textual report.
        Default ``'x'``.
    y_name : str, optional
        Display name for the function value ``y``, used in the textual
        report. Default ``'y'``.
    maxiter : int, optional
        Maximum number of iterations both for the bracketing search and
        for the bisection step. Default 1000.
    verbosity : int, optional
        Verbosity level. ``0`` suppresses output; ``>= 1`` prints the
        bracketing bounds and the final solution to ``stdout``. Default
        1.

    Returns
    -------
    x : float
        Solution of :math:`f(x) = y`.
    fx : float
        Function value :math:`f(x)` at the returned solution (should be
        close to ``y`` within the tolerance imposed by ``xtol``).

    Raises
    ------
    ValueError
        Propagated from :func:`determine_bounds` if a bracketing
        interval cannot be found within ``maxiter`` steps.
    """
    F = lambda x: f(x) - y
    a,b = determine_bounds(F,x_0,dx,maxiter)
    if verbosity > 0:
        print('Bounds are determined: %s <= %s <= %s'%(a,x_name,b))
    x = scipy.optimize.bisect(F, a, b, xtol=xtol, maxiter=maxiter)
    fx = f(x)
    if verbosity > 0:
        print('%s(%s) = %s solved:'%(y_name,x_name,y))
        print('%s = %s => %s = %s'%(x_name, x, y_name, fx))

    return x,fx
