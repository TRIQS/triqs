""" Dimension of bz mesh is reduced from 3 to 2 by
broadcasting it over mpi...

Author: H. U.R. Strand (2019) """

import numpy as np

from pytriqs.gf import MeshBrillouinZone
from pytriqs.lattice.lattice_tools import BrillouinZone
from pytriqs.lattice.lattice_tools import BravaisLattice

bl = BravaisLattice(np.eye(3), [(0,0,0)])
bz = BrillouinZone(bl)

mesh = MeshBrillouinZone(bz, 8*np.eye(3, dtype=np.int32))

import pytriqs.utility.mpi as mpi

if mpi.is_master_node():
    m = MeshBrillouinZone(bz, 8*np.eye(3, dtype=np.int32))
else:
    m = None

m = mpi.bcast(m)

print mesh
print m

assert( mesh.domain.lattice.dim == m.domain.lattice.dim )
