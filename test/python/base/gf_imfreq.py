# Copyright (c) 2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2017 Centre national de la recherche scientifique (CNRS)
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
# Authors: Olivier Parcollet, Nils Wentzell

from h5 import *
from triqs.gf import *
from triqs.utility.comparison_tests import *
import numpy as np, copy
from math import pi

beta =10 

def matsu(n) : 
   return (2*n+1)*pi/beta * 1j

g = GfImFreq(beta = beta, n_points = 100, target_shape = [])
g << inverse(iOmega_n + 2.0) 

idx_lst = list(range(3))
iw_lst  = [MatsubaraFreq(n, beta) for n in idx_lst]

X1 = np.array([ g.mesh(n).imag  for n in idx_lst])
X2 = np.array([ matsu(n).imag   for n in idx_lst])
X3 = np.array([ iw.imag         for iw in iw_lst])

assert_arrays_are_close(X1, X2)
assert_arrays_are_close(X2, X3)

Y1 = np.array([ g[Idx(n)]        for n in idx_lst])
Y2 = np.array([ g(n)             for n in idx_lst])
Y3 = np.array([ 1/(matsu(n) + 2) for n in idx_lst])
Y4 = np.array([ g[iw]            for iw in iw_lst])
Y5 = np.array([ g(iw)            for iw in iw_lst])
Y6 = np.array([ 1/(iw + 2)       for iw in iw_lst])

assert_arrays_are_close(Y1, Y2)
assert_arrays_are_close(Y2, Y3)
assert_arrays_are_close(Y3, Y4)
assert_arrays_are_close(Y4, Y5)
assert_arrays_are_close(Y5, Y6)
