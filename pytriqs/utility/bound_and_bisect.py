from __future__ import print_function
from __future__ import unicode_literals

################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2014 P.Seth
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
import scipy.optimize

def determine_bounds(F, x_0, dx, maxiter):
    """ Determine the bounds [a,b] for the zero of function F,
    using x_0 as a guess and incrementing by dx each time."""
    # Determine if function is increasing or decreasing
    if F(x_0 + dx) - F(x_0) < 0: dx *= -1.0 
    # Determine lower bound
    x = x_0; i = 0
    while not F(x) < 0.0:
        x -= dx
        i += 1
        if i > maxiter: raise ValueError("determine_bounds: cannot find lower bound.")
    a = x
    # Determine upper bound
    x = x_0; i = 0
    while not F(x) > 0.0:
        x += dx
        i += 1
        if i > maxiter: raise ValueError("determine_bounds: cannot find upper bound.")
    b = x
    return a, b
    
def bound_and_bisect(f, x_0, y=0.0, dx=1.0, xtol=1e-3, x_name='x', y_name='y', maxiter=1000, verbosity=1):
    """
    Solve F(x) = y where F is a monotonic function.
    
    Arguments :
      - F: function (real valued) to be solved
      - x_0: Initial guess for for x.
      - y: If given, F(x) = y will be solved.
      - dx: Increment to use in bound search.
      - x_name, y_name: Names of the variables x and y for the report.
      - maxiter: Maximum number of iterations for both bound searching and bisection.
    Returns :
       - A tuple (x,f(x)).
    """
    F = lambda x: f(x) - y
    a,b = determine_bounds(F,x_0,dx,maxiter)
    if verbosity > 0:
        print('Bounds are determined: %s <= %s <= %s'%(a,x_name,b))
    x = scipy.optimize.bisect(F, a, b, xtol=xtol, maxiter=maxiter)
    fx = f(x)
    if verbosity > 0:
        print('%s(%s) = %s solved:'%(y_name,x_name,y))
        print('%s = %s => %s = %s'%(x_name, x, y_name, fx))

    return x,fx
