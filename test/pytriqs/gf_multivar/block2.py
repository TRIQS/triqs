from __future__ import print_function
from pytriqs.gf import *
from pytriqs.archive import *
from itertools import izip, product

beta = 3.0
m_b = MeshImFreq(beta=beta, S = "Boson", n_max = 10)
m_f = MeshImFreq(beta=beta, S = "Fermion", n_max = 5)
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
for ref_bn, (bn, g) in izip(indices, G1): assert ref_bn == bn
for ref_bn, (bn, g) in izip(indices, G2): assert ref_bn == bn

# Properties
assert len(G1) == 6 and len(G2) == 6
assert G1.n_blocks == 6 and G2.n_blocks == 6
assert tuple(G1.indices) == indices and tuple(G2.indices) == indices
assert tuple(G1.all_indices) == tuple(i for i in product(spin_blocks, atom_blocks, *map(range,block_shape)))
assert tuple(G2.all_indices) == tuple(i for i in product(spin_blocks, atom_blocks, *map(range,block_shape)))

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
