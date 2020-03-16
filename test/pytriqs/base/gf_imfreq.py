
################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011-2017 by M. Ferrero, O. Parcollet
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
from pytriqs.archive import *
from pytriqs.gf import *
from pytriqs.utility.comparison_tests import *
import numpy as np, copy
from math import pi

beta =10 

def matsu(n) : 
   return old_div((2*n+1)*pi,beta) * 1j

g = GfImFreq(indices = [1,2], beta = beta, n_points = 100)
g << inverse(iOmega_n + 2.0) 

X = np.array([ g.mesh(n).imag  for n in range(3)])
Y = np.array([ g(n)[0,0]            for n in range(3)])

X_r = np.array([ matsu(n).imag   for n in range(3)])
Y_r = np.array([ old_div(1,(matsu(n) +2)) for n in range(3)])

assert_arrays_are_close(X, X_r) 
assert_arrays_are_close(Y, Y_r) 



