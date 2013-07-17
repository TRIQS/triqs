
################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011 by M. Ferrero, O. Parcollet
#
# TRIQS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# TRIQS. If not, see <http://www.gnu.org/licenses/>.
#
################################################################################

import numpy

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
        low  = (i for i,x in enumerate(x_array) if not( x < x_min)  ).next() 
    except StopIteration:
        return slice(0,0) # there is no x >= x_min

    try:
        high = (i for i,x in enumerate(x_array) if x > x_max ).next()
        r = slice(low,high)
    except StopIteration:
        r = slice(low,len(x_array)) # there is no x > x_max

    return r

def plot_protocol_apply(ob, opt_dict, xlims): 
    """
    Given an object ob that supports the plot protocol, it applies the protocol
    or emulate it
    """
 
    if hasattr(ob, '_plot_'): return ob._plot_(opt_dict)
    elif callable(ob):
        n_points = opt_dict.pop('n_points',100)        
        rx = opt_dict.pop('x_window', None)
        xmin, xmax = rx if rx else xlims()
        X = numpy.arange(xmin,xmax, (xmax- xmin)/float(n_points))
        Y = numpy.array( [ob(x) for x in X] )
    else:
        try: # generator x,y
            X, Y = izip (*ob)
        except: 
            raise RuntimeError, "Object can not be plotted"

    name = opt_dict.pop('name','') 
    if not name: name = str(ob)
    if numpy.iscomplexobj(Y): 
        return( [  {'type': "XY", 'xdata':X, 'ydata':Y.real, 'label': "Re " + name} , 
                   {'type': "XY", 'xdata':X, 'ydata':Y.imag, 'label': "Im " + name} ] )
    else:
        return( [ {'type': "XY", 'xdata':X, 'ydata':Y, 'label': name} ] )
 
