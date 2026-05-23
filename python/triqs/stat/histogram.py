# Copyright (c) 2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2016 Centre national de la recherche scientifique (CNRS)
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
# Authors: Thomas Ayral, Priyanka Seth, Nils Wentzell

r"""
Plot protocol registration for :class:`triqs.stat.Histogram`.

This pure-Python helper is wired up in :mod:`triqs.stat`'s
``__init__.py`` via ``plot_function_table[Histogram] = plot`` so that
histograms can be passed to ``triqs.plot.oplot``.
"""

import numpy as np

# Only the plot function, everything else is wrrapped from c++
def plot(self, optional_dict):
    r"""
    Plot protocol for Histogram objects.

    Builds the keyword-argument dictionary consumed by
    :func:`triqs.plot.protocol.plot_protocol_apply`. Registered via
    ``plot_function_table[Histogram] = plot`` in :mod:`triqs.stat`,
    so ``triqs.plot.oplot(h)`` dispatches here for a Histogram ``h``.
    Bin abscissae come from :attr:`Histogram.limits` and ``len(h)``;
    ordinates from :attr:`Histogram.data` (raw counts -- pass
    ``pdf(h)`` or ``cdf(h)`` to plot normalised distributions).

    Recognised keys in ``optional_dict`` are listed below; any
    additional keys are forwarded unchanged to the matplotlib call.

    Parameters
    ----------
    type : str, optional
        Drawing mode for the histogram. ``'XY'`` (default) plots bin
        centres as a line; ``'bar'`` draws bars centred on the bin
        centres.
    width : float, optional
        Bar width used when ``type='bar'``. Defaults to the bin
        spacing ``(b - a) / (n_bins - 1)``.

    Returns
    -------
    plot_data : list of dict
        Single-element list whose dictionary holds the keyword
        arguments forwarded by ``triqs.plot.oplot``: ``xdata``,
        ``ydata``, ``label``, ``plot_function``, and (in bar mode)
        ``width``.
    """

    plot_type = optional_dict.pop('type','XY')
    bin_centres = np.linspace(self.limits[0],self.limits[1],len(self))
    bin_width = optional_dict.pop('width', (self.limits[1] - self.limits[0])/(len(self) - 1))
    bin_edges = bin_centres - ((bin_width/2) if plot_type=='bar' else 0)

    default_dict = {'xdata': bin_edges, 
                    'ydata': self.data, 
                    'label': 'Histogram', 
                    'plot_function': 'plot' if plot_type=="XY" else 'bar'
                   }
    if plot_type=='bar' and not 'width' in optional_dict:
        default_dict['width'] = bin_width
    default_dict.update(optional_dict)

    return [default_dict]
