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
# Authors: Michel Ferrero, Alexander Hampel, Olivier Parcollet, Priyanka Seth, Nils Wentzell, tayral


__all__ = ['plt', 'oplot', 'oploti', 'oplotr', 'subplots']

import numpy
import matplotlib as mpl
import matplotlib.pyplot as plt
from .protocol import plot_protocol_apply
from matplotlib import rc

try:
    subplots = mpl.pyplot.subplots
except:
    def subplots(nrows=1, ncols=1, sharex=False, sharey=False, squeeze=True, subplot_kw=None, **fig_kw):
        print("subplots not supported")
        return plt.figure(1), [plt.subplot(nrows, ncols, x + 1) for x in range(nrows * ncols)]

def oplot(obj, *opt_list, **opt_dict):
    """
    A thin layer above pyplot.plot function that allows plotting objects with
    plot protocol as well as arrays.
    Options are the same as for the pyplot.plot function.
    """
    plt.figure(num=opt_dict.pop('num', plt.gcf().number))
    __oplot_impl(plt, plt.xlabel, plt.ylabel, plt.legend, obj, plt.xticks, plt.title, *opt_list, **opt_dict)


def oplotr(obj, *opt_list, **opt_dict):
    """
    Same as oplot, but enforce option mode='R'
    """
    opt_dict['mode'] = 'R'
    oplot(obj, *opt_list, **opt_dict)


def oploti(obj, *opt_list, **opt_dict):
    """
    Same as oplot, but enforce option mode='I'
    """
    opt_dict['mode'] = 'I'
    oplot(obj, *opt_list, **opt_dict)


mpl.axes.Axes.oplot = lambda self, obj, *opt_list, **opt_dict: __oplot_impl(self, self.set_xlabel, self.set_ylabel, self.legend, obj, self.set_xticks, self.set_title, *opt_list, **opt_dict)


def __oplot_impl(top, xlabel_fct, ylabel_fct, legend_fct, obj, xticks_fct,  title_fct, *opt_list, **opt_dict):
    """
    A thin layer above pyplot.plot function that allows plotting objects with
    plot protocol as well as arrays.
    Options are the same as for the pyplot.plot function.
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
    rc('text', usetex=True)
    rc('text.latex', preamble=r"\usepackage{amsmath}")
