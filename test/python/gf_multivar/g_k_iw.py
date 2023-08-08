# Copyright (c) 2023 Simons Foundation
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

from triqs.gf import *
from triqs.lattice import *
from h5 import *
from triqs.lattice.tight_binding import * 
from triqs.utility.h5diff import h5diff
from triqs.utility.comparison_tests import *
from numpy import cos

# === Initialize the containers and meshes

BL = BravaisLattice(units = [(1,0,0) , (0,1,0)])
mesh_k = MeshBrZone(BrillouinZone(BL), n_k=20)
mesh_iw = MeshImFreq(beta=1., S='Fermion', n_iw=100)
mesh_iw_dlr = MeshDLRImFreq(beta=1., statistic='Fermion', w_max=10, eps=1e-7)
mesh_k_iw = MeshProduct(mesh_k, mesh_iw)

# === Fill two G_k_iw

t=-0.25
U=5.0

# Dispersion
eps = lambda k: 2*t*(cos(k[0])+cos(k[1]))

# Self-Energy
Sigma = lambda iw: 0.25*U**2*inverse(iw)

G_k_iw = Gf(mesh = mesh_k_iw, target_shape = [1,1])
G_k_iw_dlr = Gf(mesh = mesh_k_iw, target_shape = [1,1])

for k in mesh_k:
    G_k_iw[k,:] << inverse(iOmega_n - eps(k) - Sigma(iOmega_n))
    G_k_iw_dlr[k,:] << inverse(iOmega_n - eps(k) - Sigma(iOmega_n))

# === Test Evaluation

for n in range(3):
  G_iw = Gf(mesh=mesh_iw, target_shape = [1,1]) 
  for k in mesh_k:
      G_iw << inverse(iOmega_n - eps(k) - Sigma(iOmega_n))
      assert_arrays_are_close(G_iw[Idx(n)], G_k_iw[k, Idx(n)])
      assert_arrays_are_close(G_iw[Idx(n)], G_k_iw(k, Idx(n)))
      assert_arrays_are_close(G_iw[Idx(n)], G_k_iw(k, n))
      assert_arrays_are_close(G_iw[Idx(n)], G_k_iw(k.index, n))
      assert_arrays_are_close(G_iw[Idx(n)], G_k_iw(k.value, n))
      assert_gfs_are_close(G_iw, G_k_iw(k.value, all))
  
      iwn = mesh_iw(n)
      assert_arrays_are_close(G_iw[Idx(n)], G_k_iw_dlr(k, iwn))
      assert_arrays_are_close(G_iw[Idx(n)], G_k_iw_dlr(k.index, iwn))
      assert_arrays_are_close(G_iw[Idx(n)], G_k_iw_dlr(k.value, iwn))
  
  G_k = Gf(mesh=mesh_k, target_shape = [1,1])
  for iw in mesh_iw:
      for k in mesh_k: G_k[k] = inverse(iw - eps(k) - Sigma(iw))
      assert_arrays_are_close(G_k[Idx(n,0,0)], G_k_iw[Idx(n,0,0), iw])
      assert_arrays_are_close(G_k[Idx(n,0,0)], G_k_iw(Idx(n,0,0), iw))
      assert_arrays_are_close(G_k[Idx(n,0,0)], G_k_iw([n,0,0], iw))
      assert_arrays_are_close(G_k[Idx(n,0,0)], G_k_iw([n,0,0], iw.index))
      assert_arrays_are_close(G_k[Idx(n,0,0)], G_k_iw([n,0,0], iw.value))
      assert_gfs_are_close(G_k, G_k_iw(all, iw))
  
      assert_arrays_are_close(G_k[Idx(n,0,0)], G_k_iw_dlr([n,0,0], iw))
      assert_arrays_are_close(G_k[Idx(n,0,0)], G_k_iw_dlr([n,0,0], iw.index))
      assert_arrays_are_close(G_k[Idx(n,0,0)], G_k_iw_dlr([n,0,0], iw.value))
      assert_gfs_are_close(G_k, G_k_iw_dlr(all, iw))

# === Test the hdf5 write/read

with HDFArchive('g_k_om.h5', 'w') as arch:
    arch['mesh_k'] = mesh_k
    arch['G'] = G_k_iw

with HDFArchive('g_k_om.h5', 'r') as arch:
    mesh_k_read = arch['mesh_k']
    G_k_iw_read = arch['G']

    assert(mesh_k == mesh_k_read)
    assert_gfs_are_close(G_k_iw, G_k_iw_read)
