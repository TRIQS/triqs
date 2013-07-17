
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

import pytriqs.utility.mpi as mpi

def dichotomy(function, x_init, y_value, precision_on_y, delta_x, max_loops = 1000, x_name="", y_name="", verbosity=1):
    """
    Solver function(x) = y_value.
    
    Arguments :
      - function : function (real valued) to be solved by dichotomy
      - x_init : Init value for x. On success, returns the new value of x
      - y_value : 
      - precision_on_y : calculation stops for abs(f(x) - y_value)<precision
      - max_loops : maximum number of loops before failure. Default is 1000
      - x_name, y_name : name of the variable x, y for the report
      - verbosity : verbosity level.

    Returns :
       - A tuple (x,y). x is the value found, y is f(x).
       - (None,None) if the calculation failed.

    """
    def sign(x):
        if x>0.0 : return 1
        if x<0.0 : return -1
        return 0
    
    mpi.report("Dichotomy adjustment of %(x_name)s to obtain %(y_name)s = %(y_value)f +/- %(precision_on_y)f"%locals() )
    PR = "    "
    if x_name=="" or y_name==""  : verbosity = max(verbosity,1)
    x=x_init;delta_x= abs(delta_x)

    # First find the bounds
    y1 = function(x)
    eps = sign(y1-y_value)
    x1=x;y2=y1;x2=x1
    nbre_loop=0
    while (nbre_loop<= max_loops) and (y2-y_value)*eps>0 and abs(y2-y_value)>precision_on_y :
        nbre_loop +=1
        x2 -=  eps*delta_x
        y2 = function(x2)
        if x_name!="" and verbosity>2:
            mpi.report("%(PR)s%(x_name)s = %(x2)f  \n%(PR)s%(y_name)s = %(y2)f"%locals())

    mpi.report("%(PR)s%(x1)f < %(x_name)s < %(x2)f"%locals())
    mpi.report("%(PR)s%(y1)f < %(y_name)s < %(y2)f"%locals())

    # Now mu is between mu1 and mu2
    yfound = y2
    # We found bounds. What if the next loop is never run ?
    # i.e. x1 or x2 are close to the solution
    # we have to know which one is the best .... 
    if abs(y1-y_value)< abs(y2-y_value) :
        x=x1
    else:
        x=x2
        
    #Now let's refine our mu....
    while (nbre_loop<= max_loops) and (abs(yfound-y_value)>precision_on_y) :
        nbre_loop +=1
        x = x1  + (x2 - x1) * (y_value - y1)/(y2-y1)
        yfound = function(x)
        if (y1-y_value)*(yfound - y_value)>0 : 
            x1 = x; y1=yfound
        else :
            x2= x;y2=yfound;
        if verbosity>2 :
            mpi.report("%(PR)s%(x1)f < %(x_name)s < %(x2)f"%locals())
            mpi.report("%(PR)s%(y1)f < %(y_name)s < %(y2)f"%locals())
    if abs(yfound - y_value) < precision_on_y :
        if verbosity>0:
            mpi.report("%(PR)s%(x_name)s found in %(nbre_loop)d iterations : "%locals())
            mpi.report("%(PR)s%(y_name)s = %(yfound)f;%(x_name)s = %(x)f"%locals())
        return (x,yfound)
    else : 
        if verbosity>0:
            mpi.report("%(PR)sFAILURE to adjust %(x_name)s  to the value %(y_value)f after %(nbre_loop)d iterations."%locals())
        return (None,None)
    
