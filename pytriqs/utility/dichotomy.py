from __future__ import division
from __future__ import unicode_literals

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

from past.utils import old_div
import pytriqs.utility.mpi as mpi
import numpy as np

def dichotomy(function, x_init, y_value, precision_on_y, delta_x,
              max_loops = 1000, x_name="", y_name="", verbosity=1):
    
    r""" Finds :math:`x` that solves :math:`y = f(x)`.
    
    Starting at ``x_init``, which is used as the lower upper/bound, 
    dichotomy finds first the upper/lower bound by adding/subtracting ``delta_x``. 
    Then bisection is used to refine :math:`x` until
    ``abs(f(x) - y_value) < precision_on_y`` or ``max_loops`` is reached.
   
    Parameters
    ----------

    function : function, real valued 
        Function :math:`f(x)`. It must take only one real parameter.
    x_init : double
        Initial guess for x. On success, returns the new value of x.
    y_value : double
        Target value for y.
    precision_on_y : double
        Stops if ``abs(f(x) - y_value) < precision_on_y``.
    delta_x : double
        :math:`\Delta x` added/subtracted from ``x_init`` until the second bound is found.
    max_loops : integer, optional
        Maximum number of loops (default is 1000).
    x_name : string, optional
        Name of variable x used for printing.
    y_name : string, optional
        Name of variable y used for printing.
    verbosity : integer, optional
        Verbosity level. 

    Returns
    -------

    (x,y) : (double, double)
        :math:`x` and :math:`y=f(x)`. Returns (None, None) if dichotomy failed.
    """
    
    mpi.report("Dichotomy adjustment of %(x_name)s to obtain %(y_name)s = %(y_value)f +/- %(precision_on_y)f"%locals() )
    PR = "    "
    if x_name == "" or y_name == ""  : verbosity = max(verbosity,1)
    x=x_init;delta_x= abs(delta_x)

    # First find the bounds
    y1 = function(x)
    eps = np.sign(y1-y_value)
    x1=x;y2=y1;x2=x1
    nbre_loop=0
    while (nbre_loop<= max_loops) and (y2-y_value)*eps>0 and abs(y2-y_value)>precision_on_y :
        nbre_loop +=1
        x2 -=  eps*delta_x
        y2 = function(x2)
        if x_name!="" and verbosity>2:
            mpi.report("%(PR)s%(x_name)s = %(x2)f  \n%(PR)s%(y_name)s = %(y2)f"%locals())
    
    # Make sure that x2 > x1
    if x1 > x2:
        x1,x2 = x2,x1
        y1,y2 = y2,y1
    
    mpi.report("%(PR)s%(x1)f < %(x_name)s < %(x2)f"%locals())
    mpi.report("%(PR)s%(y1)f < %(y_name)s < %(y2)f"%locals())

    # We found bounds.
    # If one of the two bounds is already close to the solution
    # the bisection will not run. For this case we set x and yfound.
    if abs(y1-y_value) < abs(y2-y_value) :
        yfound = y1
        x = x1
    else:
        yfound = y2
        x = x2
        
    #Now let's refine between the bounds
    while (nbre_loop<= max_loops) and (abs(yfound-y_value)>precision_on_y) :
        nbre_loop +=1
        x = x1  + old_div((x2 - x1) * (y_value - y1),(y2-y1))
        yfound = function(x)
        if (y1-y_value)*(yfound - y_value)>0 : 
            x1 = x; y1=yfound
        else :
            x2= x;y2=yfound;
        if verbosity > 2:
            mpi.report("%(PR)s%(x1)f < %(x_name)s < %(x2)f"%locals())
            mpi.report("%(PR)s%(y1)f < %(y_name)s < %(y2)f"%locals())
    if abs(yfound - y_value) < precision_on_y :
        if verbosity>0:
            mpi.report("%(PR)s%(x_name)s found in %(nbre_loop)d iterations : "%locals())
            mpi.report("%(PR)s%(y_name)s = %(yfound)f;%(x_name)s = %(x)f"%locals())
        return (x,yfound)
    else : 
        if verbosity > 0:
            mpi.report("%(PR)sFAILURE to adjust %(x_name)s to the value %(y_value)f after %(nbre_loop)d iterations."%locals())
            mpi.report("%(PR)sFAILURE returning (None, None) due to failure."%locals())
        return (None,None)
