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
# Authors: Michel Ferrero, Olivier Parcollet, Priyanka Seth, Nils Wentzell


import numpy

# dict for plotting functions for specific C++ wrapped types
# expect entries of type : plot_function
# see example in stat.__init__.py for histograms
plot_function_table = {}

def clip_array(x_array, x_min, x_max):
    """
     Given: 
      - x_array a 1d numpy array of shape (L), of ordered values
        or in fact any generator of ordered values.
      - x_min, x_max
      it returns the slice sl such that 
        * x_array[sl] is in [x_min, x_max]
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
    """
    Given an object ob that supports the plot protocol, it applies the protocol
    or emulates it (e.g. for numpy arrays).
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
