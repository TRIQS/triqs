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

from triqs.lattice import BrillouinZone, BravaisLattice, LatticePoint
from triqs.gf import MeshBrZone, MeshCycLat, Gf

import unittest

class test_cyclic_lattice(unittest.TestCase):

    def test_mesh_iteration(self):
        
        rmesh = MeshCycLat(lattice=BravaisLattice(), dims=[8, 1, 1])
        for r in rmesh:
            self.assertTrue(isinstance(r + r, LatticePoint))
            self.assertTrue(isinstance(r - r, LatticePoint))
            self.assertTrue(isinstance(-r, LatticePoint))
            self.assertTrue(isinstance(-r + r, LatticePoint))

            np.testing.assert_array_almost_equal((r + r).index, 2*np.array(r.index))
            np.testing.assert_array_almost_equal((r - r).index, 0*np.array(r.index))
            np.testing.assert_array_almost_equal((-r + r).index, 0*np.array(r.index))

            np.testing.assert_array_almost_equal(r.value, 1.0 * r)
            np.testing.assert_array_almost_equal(r + 2*r, 3*r.value)

            self.assertEqual((r + r)[0], r[0] + r[0])

    def test_gf_access_eval(self):
        rmesh = MeshCycLat(lattice=BravaisLattice(), dims=[8, 1, 1])

        g = Gf(mesh=rmesh, target_shape=[1,1])

        for r in rmesh: g[r] = sum(np.exp(-1j*r.value))

        for r in rmesh:
            self.assertEqual(g[r], g(r))
            self.assertEqual(g[r], g(r.index))
            self.assertEqual(g[r], g(r.value))

if __name__ == '__main__':
    unittest.main()
