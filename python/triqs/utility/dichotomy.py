# Copyright (c) 2013 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2019-2020 Simons Foundation
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
# Authors: Manuel, Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell


r"""
Dichotomy / regula-falsi solver for :math:`y = f(x)`.

Provides :func:`dichotomy`, a one-dimensional root finder that brackets
a sign change by stepping outward from an initial guess and then
refines the root using a linear-interpolation (regula-falsi) update
rather than plain bisection. Progress is reported through
:mod:`triqs.utility.mpi`, so the solver is safe to call from MPI runs.
"""

import triqs.utility.mpi as mpi
import numpy as np

def dichotomy(function, x_init, y_value, precision_on_y, delta_x,
              max_loops = 1000, x_name="", y_name="", verbosity=1):

    r"""
    Find :math:`x` such that :math:`f(x) = y_{\mathrm{value}}`.

    Starting from ``x_init`` (treated as either the lower or the upper
    bound depending on the local sign of :math:`f - y`), the second
    bracketing bound is found by stepping by ``delta_x`` until the sign
    of :math:`f(x) - y_{\mathrm{value}}` changes. The root is then
    refined by linear interpolation between the current bounds until
    ``abs(f(x) - y_value) < precision_on_y`` or ``max_loops`` is
    reached.

    Parameters
    ----------
    function : callable
        Real-valued function :math:`f(x)` of one real argument.
    x_init : float
        Initial guess for :math:`x`, used as one end of the initial
        bracket.
    y_value : float
        Target value :math:`y_{\mathrm{value}}` to be matched.
    precision_on_y : float
        Convergence tolerance: the iteration stops when
        ``abs(f(x) - y_value) < precision_on_y``.
    delta_x : float
        :math:`\Delta x` added to or subtracted from ``x_init`` until
        the second bracketing bound is found.
    max_loops : int, optional
        Maximum number of bracketing + bisection iterations. Default
        1000.
    x_name : str, optional
        Display name for :math:`x`, used in the textual report.
        Default ``""``.
    y_name : str, optional
        Display name for :math:`y`, used in the textual report.
        Default ``""``.
    verbosity : int, optional
        Verbosity of the textual report emitted through
        :mod:`triqs.utility.mpi`. ``0`` suppresses per-iteration output;
        ``>= 1`` prints the final answer; ``>= 3`` prints every
        intermediate :math:`x`/:math:`y` pair. Default 1.

    Returns
    -------
    x : float or None
        Solution of :math:`f(x) = y_{\mathrm{value}}`, or ``None`` if
        the iteration did not converge within ``max_loops`` steps.
    y : float or None
        Function value :math:`f(x)` at the returned solution, or
        ``None`` on failure.

    Notes
    -----
    On failure the function returns ``(None, None)`` rather than
    raising; callers are expected to check the result.

    The progress messages are printed via :func:`triqs.utility.mpi.report`,
    which suppresses output on non-master MPI ranks.
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
        x = x1  + (x2 - x1) * (y_value - y1)/(y2-y1)
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
