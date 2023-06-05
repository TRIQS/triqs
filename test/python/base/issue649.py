# Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2023 Simons Foundation
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
# Authors: Olivier Parcollet, Nils Wentzell

import copy
import numpy as np

from triqs.gf import MeshBrZone
from triqs.lattice import BrillouinZone, BravaisLattice

cell = [
    [1., 0., 2.],
    [0.5, 2.5, 1.],
    [1., 2., 0.]]

bl = BravaisLattice(cell)
bz = BrillouinZone(bl)

dims = [32, 32, 32]
bzmesh = MeshBrZone(bz, dims)

bzmesh_ref = copy.deepcopy(bzmesh) # BREAKS

units = bzmesh.bz.units
units_ref = bzmesh_ref.bz.units

np.testing.assert_array_almost_equal(units, units_ref)

