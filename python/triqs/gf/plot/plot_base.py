# Copyright (c) 2013-2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2017 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2016 Igor Krivenko
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
# Authors: Michel Ferrero, Igor Krivenko, Olivier Parcollet, Priyanka Seth, Nils Wentzell

import numpy
from warnings import warn
from triqs.plot.protocol import clip_array

def plot_base(self, opt_dict, xlabel, ylabel, X, allow_spectral_mode=False):
    r"""
    Plot protocol for Green's function objects.

    Parameters
    ----------
    opt_dict: dictionary
              MUST contain:
              - name: str
                      Name for the plotting label
              
              Can contain:
              - mode: string, default None
                      Mode to plot the Green's function in:
                      -- 'R': real part only
                      -- 'I': imaginary part only
                      -- 'S': spectral function
              - x_window: tuple, default None
                          (xmin,xmax)
    xlabel: str
            Label to apply to the x axis.
    ylabel: lambda : str -> str
            Label to apply to the y axis.
    X: list
       The x values the object can take, i.e. the mesh.
    allow_spectral_mode: boolean, default False
                         Can the spectral function be measured for this type of Green's function?

    Returns
    -------
    plot_data: list of dict
               Object passed to oplot to plot.
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

    res[0].update(opt_dict) # Add all other unused parameters to the dict
    return res

#------------------
