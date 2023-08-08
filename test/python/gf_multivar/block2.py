# Copyright (c) 2016-2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2016-2017 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020-2022 Simons Foundation
# Copyright (c) 2016 Igor Krivenko
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
# Authors: Igor Krivenko, Olivier Parcollet, Nils Wentzell

from triqs.gf import *
from h5 import *
from itertools import product

beta = 3.0
m_b = MeshImFreq(beta=beta, S = "Boson", n_iw = 10)
m_f = MeshImFreq(beta=beta, S = "Fermion", n_iw = 5)
block_mesh = MeshProduct(m_b,m_f,m_f)

spin_blocks = ('up','dn')
atom_blocks = (1,2,3)
indices = tuple(product(spin_blocks, atom_blocks))
block_shape = (2,2,2,2)

def make_vertex(sn, an):
    v = Gf(mesh = block_mesh, target_shape = block_shape)
    v.data[:] = an * (1.0 if sn == 'up' else -1.0)
    return v

blocks_list = [[make_vertex(sn,an) for an in atom_blocks] for sn in spin_blocks]

# Two ways to construct Block2Gf
G1 = Block2Gf(spin_blocks, atom_blocks, blocks_list, name = "G from list of lists")
G2 = Block2Gf(spin_blocks, atom_blocks, make_vertex, name = "G from function")

# Check iteration
for ref_bn, (bn, g) in zip(indices, G1): assert ref_bn == bn
for ref_bn, (bn, g) in zip(indices, G2): assert ref_bn == bn

# Properties
assert len(G1) == 6 and len(G2) == 6
assert G1.n_blocks == 6 and G2.n_blocks == 6
assert tuple(G1.indices) == indices and tuple(G2.indices) == indices
assert tuple(G1.all_indices) == tuple(i for i in product(spin_blocks, atom_blocks, *list(map(range,block_shape))))
assert tuple(G2.all_indices) == tuple(i for i in product(spin_blocks, atom_blocks, *list(map(range,block_shape))))

def check_value(G,ref_func):
    for bn1, bn2 in G.indices:
        assert (G[bn1,bn2].data[:] == ref_func(bn1,bn2)).all()

# Check stored values
check_value(G1,lambda bn1,bn2: bn2*(1.0 if bn1 == 'up' else -1.0))
check_value(G2,lambda bn1,bn2: bn2*(1.0 if bn1 == 'up' else -1.0))

# Arithmetics
check_value(G1+G2,lambda bn1,bn2: bn2*(2.0 if bn1 == 'up' else -2.0))
Gp = G1.copy()
Gp += G2
check_value(Gp,lambda bn1,bn2: bn2*(2.0 if bn1 == 'up' else -2.0))

check_value(G1-G2,lambda bn1,bn2: 0)
Gm = G1.copy()
Gm -= G2
check_value(Gm,lambda bn1,bn2: 0)

# check_value(G1*G2,lambda bn1,bn2: bn2*bn2)
# GG = G1.copy()
# GG *= G2
# check_value(GG,lambda bn1,bn2: bn2*bn2)

check_value(G1*5,lambda bn1,bn2: bn2*(5.0 if bn1 == 'up' else -5.0))
check_value(5*G1,lambda bn1,bn2: bn2*(5.0 if bn1 == 'up' else -5.0))
Ga = G1.copy()
Ga *= 4
check_value(Ga,lambda bn1,bn2: bn2*(4.0 if bn1 == 'up' else -4.0))
check_value(-G1,lambda bn1,bn2: -bn2*(1.0 if bn1 == 'up' else -1.0))

# Copy
G3 = G1.copy()
check_value(G3,lambda bn1,bn2: bn2*(1.0 if bn1 == 'up' else -1.0))

G3.copy_from(Gp)
check_value(G3,lambda bn1,bn2: bn2*(2.0 if bn1 == 'up' else -2.0))

G4 = G1.copy_selected_blocks(['dn'],[1,3])
check_value(G4,lambda bn1,bn2: bn2*(1.0 if bn1 == 'up' else -1.0))

G5 = G2.view_selected_blocks(['up','dn'],[2])
check_value(G4,lambda bn1,bn2: bn2*(1.0 if bn1 == 'up' else -1.0))

# <<
G1 << 3*G2
check_value(G1,lambda bn1,bn2: bn2*(3.0 if bn1 == 'up' else -3.0))

## Set item
G2['dn',3] = G1['up',2].copy()
check_value(G2,lambda bn1,bn2: 6.0 if (bn1,bn2)==('dn',3) else (bn2*(1.0 if bn1 == 'up' else -1.0)))

# HDF5
with HDFArchive('block2.h5','w') as arch:
    arch['vertex'] = G2

print(" ok ")
for n, g in G2:
    print(g.data.shape, g.mesh.components)

with HDFArchive('block2.h5','r') as arch:
  G6 = arch['vertex']
  check_value(G6,lambda bn1,bn2: 6.0 if (bn1,bn2)==('dn',3) else (bn2*(1.0 if bn1 == 'up' else -1.0)))
