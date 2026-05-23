# Copyright (c) 2013-2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2016 Centre national de la recherche scientifique (CNRS)
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
# Authors: Thomas Ayral, Michel Ferrero, Alexander Hampel, Olivier Parcollet, Priyanka Seth, Nils Wentzell


r"""
Matplotlib front-end for the TRIQS plot protocol.

This module exposes :func:`oplot`, a thin wrapper around
``matplotlib.pyplot.plot`` that consumes the protocol defined in
:mod:`triqs.plot.protocol`. Any object implementing ``_plot_`` (or
registered in :data:`~triqs.plot.protocol.plot_function_table`), as
well as plain numpy arrays, callables and iterables of ``(x, y)``
pairs, can be passed directly to :func:`oplot`.

``matplotlib.pyplot`` is re-exported as ``plt`` and
``matplotlib.pyplot.subplots`` as :data:`subplots` for convenience.
The legacy helpers :func:`oplotr`, :func:`oploti` and
``matplotlib.axes.Axes.oplot`` are deprecated and emit
``FutureWarning``.
"""

__all__ = ['plt', 'oplot', 'oploti', 'oplotr', 'subplots']

import numpy
import matplotlib as mpl
import matplotlib.pyplot as plt
from .protocol import plot_protocol_apply
from matplotlib import rc
import warnings

try:
    subplots = mpl.pyplot.subplots
except:
    def subplots(nrows=1, ncols=1, sharex=False, sharey=False, squeeze=True, subplot_kw=None, **fig_kw):
        print("subplots not supported")
        return plt.figure(1), [plt.subplot(nrows, ncols, x + 1) for x in range(nrows * ncols)]

def oplot(obj, *opt_list, axes=None, **opt_dict):
    r"""
    Plot a protocol-aware object onto a matplotlib axes.

    Thin wrapper around :func:`matplotlib.pyplot.plot` that dispatches
    ``obj`` through :func:`triqs.plot.protocol.plot_protocol_apply`,
    iterates over the curve dictionaries it returns, and forwards each
    to the appropriate matplotlib drawing function (``plot``, ``bar``,
    ...). Axis labels, title, ticks and legend toggles encoded in the
    curve dictionaries are applied to ``axes``.

    Parameters
    ----------
    obj : object
        Object to plot. Anything accepted by
        :func:`~triqs.plot.protocol.plot_protocol_apply`: a TRIQS
        object implementing the plot protocol, a numpy array, a
        callable sampled on the current x-window, or an iterable of
        ``(x, y)`` pairs.
    *opt_list
        Extra positional arguments forwarded to the matplotlib
        drawing call (e.g. a format string ``'r--'``).
    axes : matplotlib.axes.Axes, optional
        Axes to draw on. Defaults to ``plt.gca()``.
    **opt_dict
        Keyword options. Protocol-specific keys (e.g. ``mode``,
        ``x_window``, ``name``) are consumed by the dispatch in
        :func:`~triqs.plot.protocol.plot_protocol_apply`; remaining
        keys are forwarded to the matplotlib drawing call.

    Raises
    ------
    RuntimeError
        If a forwarded keyword is neither understood by the object's
        plot protocol nor a valid matplotlib line property.
    """
    axes = plt.gca() if axes is None else axes
    __oplot_impl(axes, axes.set_xlabel, axes.set_ylabel, axes.legend, obj, axes.set_xticks, axes.set_title, *opt_list, **opt_dict)


def oplotr(obj, *opt_list, **opt_dict):
    r"""
    Plot the real part of ``obj`` via :func:`oplot` with ``mode='R'``.

    .. deprecated:: 3.3
        Use ``oplot(obj.real, ...)`` instead. Emits ``FutureWarning``.

    Parameters
    ----------
    obj : object
        See :func:`oplot`.
    *opt_list, **opt_dict
        Forwarded to :func:`oplot`. ``mode`` is overwritten with
        ``'R'``.
    """
    warnings.warn("oplotr is deprecated, use oplot(G.real, ...) instead", FutureWarning)
    opt_dict['mode'] = 'R'
    oplot(obj, *opt_list, **opt_dict)


def oploti(obj, *opt_list, **opt_dict):
    r"""
    Plot the imaginary part of ``obj`` via :func:`oplot` with ``mode='I'``.

    .. deprecated:: 3.3
        Use ``oplot(obj.imag, ...)`` instead. Emits ``FutureWarning``.

    Parameters
    ----------
    obj : object
        See :func:`oplot`.
    *opt_list, **opt_dict
        Forwarded to :func:`oplot`. ``mode`` is overwritten with
        ``'I'``.
    """
    warnings.warn("oploti is deprecated, use oplot(G.imag, ...) instead", FutureWarning)
    opt_dict['mode'] = 'I'
    oplot(obj, *opt_list, **opt_dict)

def __axoplot_impl(top, xlabel, ylabel, legend, obj, xticks, title, *opt_list, **opt_dict):
    warnings.warn("ax.oplot is deprecated, use oplot(obj, axes=ax) instead", FutureWarning, stacklevel=2)
    return __oplot_impl(top, xlabel, ylabel, legend, obj, xticks, title, *opt_list, **opt_dict)

mpl.axes.Axes.oplot = lambda self, obj, *opt_list, **opt_dict: __axoplot_impl(self, self.set_xlabel, self.set_ylabel, self.legend, obj, self.set_xticks, self.set_title, *opt_list, **opt_dict)


def __oplot_impl(top, xlabel_fct, ylabel_fct, legend_fct, obj, xticks_fct,  title_fct, *opt_list, **opt_dict):
    """
    Internal: drive the plot protocol on ``obj`` and dispatch each
    curve dictionary to the matplotlib drawing call ``top.<plot_function>``.
    Axis-label, title, ticks and legend setters are passed in as
    free-function arguments so the same implementation backs both
    :func:`oplot` and the deprecated ``Axes.oplot`` shim.
    """

    for curve_dict in plot_protocol_apply(obj, opt_dict, plt.xlim):

        plt_fct = getattr(top, curve_dict.pop('plot_function', 'plot'))
        X = curve_dict.pop('xdata')
        Y = curve_dict.pop('ydata')

        # Sort out axis labels and legend
        xlabel = curve_dict.pop('xlabel',None)
        ylabel = curve_dict.pop('ylabel',None)
        xticks = curve_dict.pop('xticks',None)
        title = curve_dict.pop('title',None)
        if xlabel: xlabel_fct(xlabel)
        if ylabel: ylabel_fct(ylabel)
        if title: title_fct(title)
        if xticks: xticks_fct(*xticks, rotation=30)
        is_legend = curve_dict.pop('legend',True)

        try:
         if "zdata" in list(curve_dict.keys()):
            Z = curve_dict.pop('zdata')
            plt_fct(X, Y, Z, *opt_list, **curve_dict)
         else:
            plt_fct(X, Y, *opt_list, **curve_dict)
        except TypeError as e:
            import re
            m = re.search('(?<=There is no line property )"(.*)"', str(e))
            if m:
                raise RuntimeError("Plot option %s not understood: it is neither an option of the object to be plotted, nor a matplotlib option." % m.group(0))
            else:
                raise

        if is_legend: legend_fct(loc=1)  # legend is built from the label

def use_amsmath():
    r"""
    Configure matplotlib to render math labels with LaTeX + ``amsmath``.

    Sets ``rcParams['text.usetex'] = True`` and prepends
    ``\usepackage{amsmath}`` to ``rcParams['text.latex.preamble']``,
    enabling ``\text{}``, ``\boldsymbol{}``, aligned environments and
    other ``amsmath`` features in axis labels and legends. Requires a
    working LaTeX installation on ``PATH``.
    """
    rc('text', usetex=True)
    rc('text.latex', preamble=r"\usepackage{amsmath}")
