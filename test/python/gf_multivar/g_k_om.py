# Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
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
# Authors: Olivier Parcollet, Nils Wentzell, tayral

from triqs.gf import *
from triqs.lattice import *
from h5 import *
from triqs.lattice.tight_binding import * 
from triqs.utility.h5diff import h5diff
from triqs.utility.comparison_tests import assert_gfs_are_close
from numpy import cos

# === Initialize the containers and meshes

BL = BravaisLattice(units = [(1,0,0) , (0,1,0) ]) #square lattice
nk=20
mesh_k = MeshBrZone(BrillouinZone(BL), nk)
mesh_iw = MeshImFreq(beta=1., S="Fermion", n_iw=100) #not used  (just demo)
mesh_k_iw = MeshProduct(mesh_k, mesh_iw) # not used (just demo)

mesh_w = MeshReFreq(-5,5, 100)
mesh_k_w = MeshProduct(mesh_k, mesh_w)

for m in mesh_k: print(m)
    
# === Fill two G_k_w

G_w = GfReFreq(mesh=mesh_w, target_shape = [1,1]) 

t=-0.25
U=5.0
eta=0.2

G_k_w = Gf(mesh = mesh_k_w, target_shape = [1,1])
G_k_w_Mott = Gf(mesh = mesh_k_w, target_shape= [1,1])

for k in G_k_w.mesh.components[0]:
    G_w << inverse(iOmega_n-2*t*(cos(k[0])+cos(k[1]))+eta*1j)
    G_k_w[k,:].data[:] = G_w.data

    G_w << inverse(iOmega_n-2*t*(cos(k[0])+cos(k[1]))+eta*1j - 0.25*U**2*inverse(iOmega_n+eta*1j))
    G_k_w_Mott[k,:].data[:] = G_w.data

# === Test the hdf5 write/read

with HDFArchive('g_k_om.h5', 'w') as arch:
    arch['mesh_k'] = mesh_k
    arch['G'] = G_k_w

with HDFArchive('g_k_om.h5', 'r') as arch:
    mesh_k_read = arch['mesh_k']
    G_k_w_read = arch['G']

    assert(mesh_k == mesh_k_read)
    assert_gfs_are_close(G_k_w, G_k_w_read)
