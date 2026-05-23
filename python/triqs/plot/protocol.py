# Copyright (c) 2013-2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2016 Centre national de la recherche scientifique (CNRS)
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
# Authors: Michel Ferrero, Alexander Hampel, Olivier Parcollet, Priyanka Seth, Nils Wentzell


r"""
Definition of the TRIQS plot protocol.

The plot protocol is the convention TRIQS objects use to describe
themselves to a plotting backend (typically
:mod:`triqs.plot.mpl_interface`). An object supports the protocol
if it satisfies one of:

* it has a method ``_plot_(opt_dict)`` returning a list of curve
  dictionaries, or
* its type is registered in :data:`plot_function_table`, mapping to
  a free function ``f(obj, opt_dict)`` returning the same.

Each returned curve dictionary holds the keyword arguments forwarded
to matplotlib by :func:`~triqs.plot.mpl_interface.oplot`: at minimum
``xdata`` and ``ydata``, and optionally ``label``, ``plot_function``
(``'plot'``, ``'bar'``, ...), ``xlabel``, ``ylabel``, ``title``,
``xticks``, ``zdata``, ``legend``, plus any line-property kwargs.
"""

import numpy

plot_function_table = {}
"""dict : Registry mapping ``type`` to a plot-protocol function.

Used for types that cannot host a ``_plot_`` method directly, such as
C++-wrapped classes. Each value is a callable ``f(obj, opt_dict)`` that
returns a list of curve dictionaries as described in the module
docstring.

Examples
--------
Registering :class:`triqs.stat.Histogram`, as done in
``triqs/stat/__init__.py``::

    from .histograms import Histogram
    from .histogram import plot
    from triqs.plot.protocol import plot_function_table
    plot_function_table[Histogram] = plot
"""

def clip_array(x_array, x_min, x_max):
    r"""
    Return the slice selecting entries of an ordered array within a window.

    Parameters
    ----------
    x_array : array_like
        1D sequence of monotonically non-decreasing values. Any
        iterable that yields values in order is accepted.
    x_min : float
        Lower bound of the window (inclusive).
    x_max : float
        Upper bound of the window (inclusive).

    Returns
    -------
    slice
        Slice ``sl`` such that every element of ``x_array[sl]`` lies in
        ``[x_min, x_max]``. Returns ``slice(0, 0)`` when no element
        satisfies ``x >= x_min``.

    Raises
    ------
    AssertionError
        If ``x_max < x_min``.
    """
    assert x_max >= x_min, "Windowing error"

    try:
        low = next((i for i, x in enumerate(x_array) if not(x < x_min)))
    except StopIteration:
        return slice(0, 0)  # there is no x >= x_min

    try:
        high = next((i for i, x in enumerate(x_array) if x > x_max))
        r = slice(low, high)
    except StopIteration:
        r = slice(low, len(x_array))  # there is no x > x_max

    return r


def plot_protocol_apply(ob, opt_dict, xlims):
    r"""
    Apply the plot protocol to an arbitrary object.

    Dispatches ``ob`` to the most specific plot-protocol handler
    available, in this order:

    1. ``ob._plot_(opt_dict)`` if ``ob`` exposes a ``_plot_`` method.
    2. ``plot_function_table[type(ob)](ob, opt_dict)`` if ``type(ob)``
       is registered in :data:`plot_function_table`.
    3. If ``ob`` is callable, sample it on a uniform grid of
       ``n_points`` (default 100) points spanning ``x_window`` (taken
       from ``opt_dict`` or ``xlims()`` as a fallback).
    4. Otherwise, treat ``ob`` as an iterable of ``(x, y)`` pairs.

    Complex-valued ordinates are split into separate real and imaginary
    curves labelled ``"Re <name>"`` and ``"Im <name>"``.

    Parameters
    ----------
    ob : object
        Object to be plotted. Must implement the plot protocol, be
        registered in :data:`plot_function_table`, be callable, or be
        an iterable of ``(x, y)`` pairs.
    opt_dict : dict
        Plotting options. Keys consumed here (``n_points``,
        ``x_window``, ``name``) are popped; remaining keys are
        forwarded inside the returned curve dictionaries.
    xlims : callable
        Zero-argument callable returning ``(xmin, xmax)`` for the
        sampling window when ``ob`` is callable and no ``x_window``
        was given. Typically ``matplotlib.pyplot.xlim``.

    Returns
    -------
    list of dict
        Curve dictionaries as described in the module docstring.

    Raises
    ------
    RuntimeError
        If ``ob`` does not match any of the supported forms.
    """

    # the object can have a native plot function defined in the class
    if hasattr(ob, '_plot_'):
        return ob._plot_(opt_dict)
    # or registered in the plot_function_table variable
    elif type(ob) in plot_function_table:
        return plot_function_table[type(ob)](ob, opt_dict)
    elif callable(ob):
        n_points = opt_dict.pop('n_points', 100)
        rx = opt_dict.pop('x_window', None)
        xmin, xmax = rx if rx else xlims()
        X = numpy.arange(xmin, xmax, (xmax - xmin) / float(n_points))
        Y = numpy.array([ob(x) for x in X])
    else:
        try: # generator x,y
            X, Y = zip(*ob)
        except:
            raise RuntimeError("Object cannot be plotted")

    name = opt_dict.pop('name', str(ob))
    if numpy.iscomplexobj(Y):
        return([{'xdata': X, 'ydata': Y.real, 'label': "Re " + name},
                {'xdata': X, 'ydata': Y.imag, 'label': "Im " + name}])
    else:
        return([{'xdata': X, 'ydata': Y, 'label': name}])
