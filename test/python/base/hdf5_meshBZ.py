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
# Authors: Nils Wentzell

import numpy as np
from triqs.gf import *
from h5 import HDFArchive
from triqs.lattice import BrillouinZone, BravaisLattice

bz = BrillouinZone(BravaisLattice([[1,0],[0,1]]))
bzmesh = MeshBrillouinZone(bz, n_k=4)

beta = 1.2345
fmesh = MeshImFreq(beta=beta, S='Fermion', n_max=8)
bmesh = MeshImFreq(beta=beta, S='Boson', n_max=6)
prodmesh = MeshProduct(bzmesh, bmesh, fmesh, fmesh)

ek = Gf(mesh=bzmesh, target_shape=[1, 1])
ek.data[:] = np.random.random(ek.data.shape)

chi = Gf(mesh=prodmesh, target_shape=[1, 1, 1, 1])
chi.data[:] = np.random.random(chi.data.shape)

filename = 'test_bz_h5.h5'

with HDFArchive(filename, 'w') as s:
    s['ek'] = ek
    s['chi'] = chi

with HDFArchive(filename, 'r') as s:
    ek_ref = s['ek']
    chi_ref = s['chi']

np.testing.assert_array_almost_equal(ek.data, ek_ref.data)
np.testing.assert_array_almost_equal(chi.data, chi_ref.data)
