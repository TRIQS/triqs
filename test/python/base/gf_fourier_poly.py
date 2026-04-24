# Copyright (c) 2026 Simons Foundation
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
#     https://www.gnu.org/licenses/gpl-3.0.txt
#
# Authors: Nils Wentzell

import unittest

import numpy as np
from h5 import HDFArchive

from triqs.gfs import Gf
from triqs.mesh import MeshFourierPoly


class test_mesh_fourier_poly(unittest.TestCase):

    R_list = [(1, 0, 0), (-1, 0, 0),
              (0, 1, 0), (0, -1, 0),
              (0, 0, 1), (0, 0, -1)]

    def test_mesh_basics(self):
        m = MeshFourierPoly(self.R_list)
        self.assertEqual(len(m), 6)
        self.assertEqual(list(m.R_list), [list(R) for R in self.R_list])
        self.assertEqual(np.asarray(m.R_mat).shape, (6, 3))

        # R-vector -> index lookup
        self.assertEqual(m.to_index([0, 1, 0]), 2)
        self.assertEqual(list(m.to_value(2)), [0, 1, 0])
        self.assertEqual(list(m[0].value), [1, 0, 0])

        # iteration yields mesh_points in R_list order
        self.assertEqual([list(mp.value) for mp in m],
                         [list(R) for R in self.R_list])

    def test_mesh_h5_roundtrip(self):
        m = MeshFourierPoly(self.R_list)
        with HDFArchive('gf_fourier_poly_mesh.h5', 'w') as R: R['m'] = m
        with HDFArchive('gf_fourier_poly_mesh.h5', 'r') as R: m2 = R['m']
        self.assertEqual(m, m2)

    def test_mesh_equality(self):
        m1 = MeshFourierPoly(self.R_list)
        m2 = MeshFourierPoly(self.R_list)
        m3 = MeshFourierPoly([(1, 0, 0), (-1, 0, 0)])
        self.assertEqual(m1, m2)
        self.assertNotEqual(m1, m3)


class test_gf_fourier_poly(unittest.TestCase):

    R_list = [(1, 0, 0), (-1, 0, 0),
              (0, 1, 0), (0, -1, 0),
              (0, 0, 1), (0, 0, -1)]

    def _cubic_hopping_gf(self, t=1.0, norb=2):
        m = MeshFourierPoly(self.R_list)
        g = Gf(mesh=m, target_shape=(norb, norb))
        for mp in m:
            g[mp] = t * np.eye(norb)
        return g, m, t

    def test_evaluate_gamma(self):
        g, m, t = self._cubic_hopping_gf()
        # Six nearest-neighbor hops at Gamma: sum_R cos(0) * diag(t) = 6t
        self.assertTrue(np.allclose(g((0.0, 0.0, 0.0)), 6 * t * np.eye(2)))

    def test_evaluate_generic_k(self):
        g, m, t = self._cubic_hopping_gf()
        k = (0.25, 0.5, 0.05)
        expected = 2 * t * (np.cos(2 * np.pi * 0.25)
                            + np.cos(2 * np.pi * 0.5)
                            + np.cos(2 * np.pi * 0.05))
        self.assertTrue(np.allclose(g(k), expected * np.eye(2), atol=1e-10))

    def test_eigenvalues(self):
        g, _, t = self._cubic_hopping_gf()
        k = (0.33, 0.5, 0.33)
        ens = np.linalg.eigvalsh(g(k))
        ref = 2 * t * (np.cos(2 * np.pi * 0.33)
                       + np.cos(2 * np.pi * 0.5)
                       + np.cos(2 * np.pi * 0.33))
        self.assertTrue(np.allclose(ens, [ref, ref], atol=1e-10))

    def test_algebra(self):
        g, _, _ = self._cubic_hopping_gf()
        self.assertTrue(np.allclose((g + g).data, 2 * g.data))
        self.assertTrue(np.allclose((2.0 * g).data, 2 * g.data))
        self.assertTrue(np.allclose((-g).data, -g.data))
        self.assertTrue(np.allclose((g - g).data, 0.0))

    def test_h5_roundtrip(self):
        g, _, _ = self._cubic_hopping_gf()
        with HDFArchive('gf_fourier_poly.h5', 'w') as R: R['g'] = g
        with HDFArchive('gf_fourier_poly.h5', 'r') as R: g2 = R['g']
        self.assertTrue(np.allclose(g.data, g2.data))
        self.assertEqual(list(g.mesh.R_list), list(g2.mesh.R_list))
        # evaluation agrees post-roundtrip
        k = (0.25, 0.5, 0.05)
        self.assertTrue(np.allclose(g(k), g2(k)))

    def test_scalar_valued(self):
        m = MeshFourierPoly(self.R_list)
        g = Gf(mesh=m, target_shape=())
        for i, mp in enumerate(m):
            g[mp] = float(i)
        for i, mp in enumerate(m):
            self.assertEqual(g[mp], float(i))

        # k-point evaluation: sum_R coeff(R) * exp(2 pi i k . R)
        k = (0.25, 0.5, 0.05)
        expected = sum(float(i) * np.exp(2j * np.pi * np.dot(k, R))
                       for i, R in enumerate(self.R_list))
        self.assertTrue(np.isclose(g(k), expected, atol=1e-12))

    def test_tensor3_valued(self):
        m = MeshFourierPoly(self.R_list)
        g = Gf(mesh=m, target_shape=(2, 2, 2))
        # non-constant R-dependence so the k-sum is non-trivial
        for mp in m:
            R = np.array(mp.value)
            g[mp] = (R[0] + 1.0) * np.ones((2, 2, 2))
            g[mp][0, 1, 1] += 1j * R[1]
        # constant-component sanity check
        self.assertTrue(np.allclose(g.data[:, 0, 0, 0],
                                    [R[0] + 1.0 for R in self.R_list]))

        # k-point evaluation, elementwise against explicit sum
        k = (0.15, 0.25, 0.05)
        expected = sum(g.data[i] * np.exp(2j * np.pi * np.dot(k, R))
                       for i, R in enumerate(self.R_list))
        self.assertTrue(np.allclose(g(k), expected, atol=1e-12))


if __name__ == '__main__':
    unittest.main()
