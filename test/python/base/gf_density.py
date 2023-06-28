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

iw_mesh = MeshImFreq(beta=beta, S='Fermion', n_iw=1000)
g_iw = GfImFreq(mesh=iw_mesh, target_shape=[1,1])

print("==============================================")

for eps in np.random.random(10):
    g_iw << inverse(iOmega_n - eps)
    km = make_zero_tail(g_iw, 2)
    km[1] = np.eye(g_iw.target_shape[0])
    n = np.squeeze(g_iw.density(km))
    n_ref = fermi(eps, beta)
    np.testing.assert_almost_equal(n, n_ref)


print("==============================================")
# -- Test real frequency density for free Gf

energies = np.diag(np.array([-3, -2, -1],dtype=complex))

mesh = MeshReFreq(n_w=20001, window=(-10,3))
gf_w = Gf(mesh=mesh, target_shape=[3,3])
eta = 0.001

gf_w[:,:] << inverse(Omega + 1j*eta - energies )

assert( np.allclose(np.eye(3), gf_w.density(beta=1000), rtol=1e-3 ) )

assert( gf_w.total_density(beta=1000).real - 3.0 < 1e-3)