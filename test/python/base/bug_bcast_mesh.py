# Copyright (c) 2019-2020 Simons Foundation
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

""" Dimension of bz mesh is reduced from 3 to 2 by
broadcasting it over mpi...

Author: H. U.R. Strand (2019) """

import numpy as np

from triqs.gf import MeshBrZone
from triqs.lattice.lattice_tools import BrillouinZone
from triqs.lattice.lattice_tools import BravaisLattice

bl = BravaisLattice(np.eye(3), [(0,0,0)])
bz = BrillouinZone(bl)

mesh = MeshBrZone(bz, 8*np.eye(3, dtype=np.int32))

import triqs.utility.mpi as mpi

if mpi.is_master_node():
    m = MeshBrZone(bz, 8*np.eye(3, dtype=np.int32))
else:
    m = None

m = mpi.bcast(m)

print(mesh)
print(m)

assert(mesh.domain.lattice.ndim == m.domain.lattice.ndim)
