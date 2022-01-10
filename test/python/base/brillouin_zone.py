# Copyright (c) 2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2016 Centre national de la recherche scientifique (CNRS)
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
# Authors: Nils Wentzell, tayral

from numpy import *
from numpy.linalg import *
from triqs.gf import *
from triqs.lattice import *
from triqs.lattice.lattice_tools import *
from math import pi

import unittest

class test_brillouin_zone(unittest.TestCase):

    def test_mesh_construction(self):

        # Helper function
        def run(units, per_mat):
            bl     = BravaisLattice(units)
            bz     = BrillouinZone(bl)
            k_mesh = MeshBrillouinZone(bz, per_mat)

            p1, p2, p3 = per_mat
            k1, k2, k3 = bz.units()
            assert array_equal(k_mesh.locate_neighbours(k1), p1)
            assert array_equal(k_mesh.locate_neighbours(k2), p2)
            assert array_equal(k_mesh.locate_neighbours(k3), p3)
            assert norm(dot(bl.units, bz.units().T) - eye(3)*2*pi) < 1e-14

        # ---- square lattice : 16 x 16 x 1
        units = array([[1.,0.,0.],[0.,1.,0.],[0.,0.,1.]], float)
        per_mat = array([[16, 0, 0],[0, 16, 0],[0, 0, 1]], int32)
        run(units, per_mat)

        # ---- triangular lattice : 6 x 6 x 1
        units = array([[1.,0.,0.],[0.5,sqrt(3)/2.,0.],[0.,0.,1.]], float)
        per_mat = array([[6, 0, 0],[0, 6, 0],[0, 0, 1]], int32)
        run(units, per_mat)

        # ---- random ONB and per_mat
        random.seed(seed=12345)
        from scipy.stats import ortho_group
        units = ortho_group.rvs(3) # Random orthonormal 3x3 matrix
        per_mat = diag(random.randint(1, 10, size=(3,), dtype=int32))
        while linalg.det(per_mat) == 0:
            per_mat = diag(random.randint(1, 10, size=(3,), dtype=int32))
        run(units, per_mat)

if __name__ == '__main__':
    unittest.main()
