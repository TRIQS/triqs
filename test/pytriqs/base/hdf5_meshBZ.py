from __future__ import unicode_literals
import numpy as np
from pytriqs.gf import *
from pytriqs.archive import HDFArchive
from pytriqs.lattice import BrillouinZone, BravaisLattice

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
