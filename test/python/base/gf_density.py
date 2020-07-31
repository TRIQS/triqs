# Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2020 Simons Foundation
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
# Authors: Hugo U. R. Strand, Nils Wentzell

import numpy as np
from triqs.gf import *

def fermi(eps, beta):
    return 1./(1. + np.exp(beta * eps))

beta = 50.0

# -- Test Matsubara frequency density for free Gf

g_iw = GfImFreq(beta=beta, indices=[0], n_points=1000)

print("==============================================")

print(g_iw.density())

for eps in np.random.random(10):
    g_iw << inverse(iOmega_n - eps)
    km = make_zero_tail(g_iw, 2)
    km[1] = np.eye(g_iw.target_shape[0])
    n = np.squeeze(g_iw.density(km))
    n_ref = fermi(eps, beta)
    np.testing.assert_almost_equal(n, n_ref)
