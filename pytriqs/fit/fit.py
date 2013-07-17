
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

from scipy.optimize import leastsq
import numpy as np, inspect as ins

class Fit:
    """
    A simple general functional fit of a X,Y plot

    Given a function f(x, p0,p1,p2 ...) with parameters p0, ..., p2, and an init guess
    it adjust the parameters with least square method.

    The fitting is done at construction

    `self.param` is the tuple of adjusted parameters.

    The object is callable : `self(x) = f(x, *self.param)`, so it can be plotted e.g.

    """
    def __init__ (self, x_array, y_array, fitter, p0 = None ) :
        """
        :param x_array,y_array: curve to fit, as two 1d numpy arrays
        :param fitter: a tuple (F, name, init_value_default) where :

                         * F is a function : `(x, *param_tuple)` -> y, which act on numpy arrays x and y
                         * name is string for which name%param_tuple gives the TeX representation of the function
                         * init_value_default is the default init point of the minimization

        :param p0: init guess of the fit. If None, uses the init_value_default of the function.

        """
        self.function, self.fname, p00 = fitter
        assert len(ins.getargspec(self.function)[0])== len(p00) + 1, "error in number of parameters"
        assert len(y_array) == len(x_array)
        assert len(y_array) > len (p00)
        errfunc = lambda x :  np.abs ( self.function(x_array,*x)  - y_array[:])
        self.param, success = leastsq(errfunc, p0 if p0 else p00 )

    def __str__ (self) : return (self.fname%tuple(self.param) or 'Fit').replace("+ -","-")
    def __repr__ (self) : return str(self)
    def __repr_tex__ (self) : return str(self)
    def __call__ (self,x) : return self.function(x,*self.param)


# a collection of useful fit ...

linear             =  lambda X, a,b   : a * X + b,             r"$%f x + %f$"          , (1,1)
quadratic          =  lambda X, a,b,c : (a * X + b)*X + c,     r"$%f x^2 + %f x + %f$" , (0,1,1)


