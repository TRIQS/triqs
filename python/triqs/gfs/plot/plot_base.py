# Copyright (c) 2013-2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2017 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020-2023 Simons Foundation
# Copyright (c) 2016 Igor Krivenko
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
# Authors: Michel Ferrero, Igor Krivenko, Olivier Parcollet, Priyanka Seth, Nils Wentzell

r"""Shared backend for the per-mesh plot functions in :mod:`.one_var`."""

import numpy
from warnings import warn
from triqs.plot.protocol import clip_array

def plot_base(self, opt_dict, xlabel, ylabel, X, allow_spectral_mode=False):
    r"""Build the curve dictionaries consumed by 
    :func:`~triqs.plot.mpl_interface.oplot`.

    The following plotting options are consumed by this function:

    - ``name`` — label prefix for the legend (defaults to
      ``self.name``).
    - ``mode`` — one of ``'R'`` (real), ``'I'`` (imaginary),
      ``'S'`` (spectral function) or empty (both real and
      imaginary). Default ``''``.
    - ``x_window`` — ``(xmin, xmax)`` clipping window on the
      mesh axis.

    Any other entries are forwarded to every curve dictionary.

    Parameters
    ----------
    self : Gf
        Green's function being plotted.
    opt_dict : dict
        Plot options.
    xlabel : str
        ``xlabel`` for the plot.
    ylabel : callable
        ``ylabel(name) -> str`` producing the y-axis label from the
        Green's function name.
    X : array-like
        Mesh values for the x axis.
    allow_spectral_mode : bool, optional
        If ``True``, ``mode='S'`` is accepted and returns
        :math:`-\frac{1}{\pi}\,\mathrm{Im}\, G`. Default ``False``.

    Returns
    -------
    list of dict
        One dictionary per target-space element to be plotted, with
        keys ``xlabel``, ``ylabel``, ``xdata``, ``ydata``, ``label`` 
        and any pass-through options from ``opt_dict``.

    Raises
    ------
    ValueError
        For an unknown ``mode`` value, or for ``mode='S'`` when
        ``allow_spectral_mode`` is ``False``.
    """

    assert 'name_prefix' not in opt_dict, "name_prefix is deprecated"
    #if 'name' not in opt_dict: 
    #    warnings.warn("oplot REQUIRES a name = for making the legend and labels. Using self.name, but it is deprecated and WILL BE REMOVED")
    name = opt_dict.pop('name', self.name)
    rx = opt_dict.pop('x_window', None)
    X = numpy.array(X).real
    sl = clip_array(X, *rx) if rx else slice(len(X)) # the slice due to clip option x_window

    def mdic(prefix, f):
        from itertools import product
        ind_range = product(*list(map(range,reversed(self.target_shape))))
        make_label = lambda ind: "%s%s %s" % (prefix,name,"_".join(map(str, reversed(ind))))
        make_data_sl = lambda ind: (sl,) + tuple(reversed(ind))
        return [{'xlabel': xlabel,
                 'ylabel': ylabel(name),
                 'xdata': X[sl],
                 'label': make_label(ind),
                 'ydata': f(self.data[make_data_sl(ind)])} for ind in ind_range]

    # if data is real, overrule
    mode = opt_dict.pop('mode', '')
    if self.data.dtype == numpy.float64 : 
        res = mdic('', lambda x: x)    
    elif mode == '':
        res = mdic('Re ', lambda x: x.real) + mdic('Im ', lambda x: x.imag)
    elif mode == 'R':
        res = mdic('Re ', lambda x: x.real)
    elif mode == 'I':
        res = mdic('Im ', lambda x: x.imag)
    elif mode == 'S':
        if allow_spectral_mode:
            res = mdic('', lambda x: -1 / numpy.pi * x.imag)
        else:
            raise ValueError("Cannot measure the spectral function for this type of Green's function.")
    else:
        raise ValueError("The 'mode' flag is meaningless. Expected 'R', 'I', or 'S' and I got %s." % mode)

    for r in res: r.update(opt_dict) # Add all other unused parameters to the dictionaries
    return res

#------------------
