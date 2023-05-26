# Copyright (c) 2023 Hugo U.R. Strand
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
# Authors: Hugo U.R. Strand

import numpy as np

from triqs.lattice import BrillouinZone, BravaisLattice
from triqs.gf import MeshBrZone, MeshCycLat

import unittest

class test_cyclic_lattice(unittest.TestCase):

    def test_mesh_iteration(self):
        
        units = np.eye(3)
        per_mat = np.diag([8, 1, 1])
        
        bl = BravaisLattice(units)

        rm = MeshCycLat(bl, per_mat)
        print(rm)

        for r in rm:
            print(r)

        bz = BrillouinZone(bl)

        km = MeshBrZone(bz, per_mat)
        print(km)
        
        for k in km:
            print(k)


if __name__ == '__main__':
    unittest.main()
