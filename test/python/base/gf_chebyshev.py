# Copyright (c) 2025 Simons Foundation
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
import os

from triqs.gfs import Gf
from triqs.mesh import MeshChebyshev


def test_function(tau, beta, omega):
    """Smooth test function for interpolation"""
    return 1.0 / (1.0 + np.exp(omega * (tau - beta / 2.0)))


class test_chebyshev_mesh(unittest.TestCase):

    def test_mesh_basics(self):
        """Test basic mesh construction and properties"""
        beta = 10.0
        N = 20
        mesh = MeshChebyshev(beta, 'Fermion', N)

        self.assertEqual(len(mesh), N)
        self.assertEqual(mesh.beta, beta)
        self.assertEqual(mesh.statistic, 'Fermion')

        # Check points are in [0, beta]
        for mp in mesh:
            self.assertGreaterEqual(mp.value, 0.0)
            self.assertLessEqual(mp.value, beta)

        # Check ascending order
        values = [mp.value for mp in mesh]
        self.assertEqual(values, sorted(values))

    def test_mesh_points_property(self):
        """Test access to Chebyshev points and weights"""
        beta = 10.0
        N = 15
        mesh = MeshChebyshev(beta, 'Fermion', N)

        points = mesh.points
        weights = mesh.weights

        self.assertEqual(len(points), N)
        self.assertEqual(len(weights), N)

        # Points should be in [0, beta]
        self.assertTrue(np.all(points >= 0))
        self.assertTrue(np.all(points <= beta))

    def test_gf_scalar_valued(self):
        """Test scalar-valued GF on Chebyshev mesh"""
        beta = 10.0
        N = 25
        mesh = MeshChebyshev(beta, 'Fermion', N)

        g = Gf(mesh=mesh, target_shape=[])

        # Fill with simple linear function f(tau) = tau
        for mp in mesh:
            g[mp] = mp.value

        # Test interpolation at midpoint
        tau_mid = beta / 2.0
        self.assertAlmostEqual(np.real(g(tau_mid)), tau_mid, places=10)

    def test_gf_matrix_valued(self):
        """Test matrix-valued GF on Chebyshev mesh"""
        beta = 5.0
        N = 15
        mesh = MeshChebyshev(beta, 'Fermion', N)

        g = Gf(mesh=mesh, target_shape=[2, 2])
        g.data[:] = 0.0

        # Fill with g(tau) = tau * I
        for mp in mesh:
            g[mp] = np.eye(2) * mp.value

        # Test interpolation
        tau_mid = beta / 2.0
        g_mid = g(tau_mid)
        np.testing.assert_almost_equal(np.real(g_mid[0, 0]), tau_mid, decimal=10)
        np.testing.assert_almost_equal(np.real(g_mid[1, 1]), tau_mid, decimal=10)

    def test_interpolation_accuracy(self):
        """Test interpolation accuracy with smooth function"""
        beta = 10.0
        omega = 1.0
        N = 30
        mesh = MeshChebyshev(beta, 'Fermion', N)

        g = Gf(mesh=mesh, target_shape=[])

        # Fill with test function values
        for mp in mesh:
            g[mp] = test_function(mp.value, beta, omega)

        # Test interpolation at various tau values
        tau_test = np.linspace(0.01, beta - 0.01, 50)
        max_err = 0.0
        for tau in tau_test:
            exact = test_function(tau, beta, omega)
            interp = np.real(g(tau))
            err = abs(exact - interp)
            max_err = max(max_err, err)

        # Chebyshev should have good convergence
        self.assertLess(max_err, 1e-7)

    def test_interpolation_at_mesh_points(self):
        """Test that interpolation at mesh points is exact"""
        beta = 10.0
        N = 20
        mesh = MeshChebyshev(beta, 'Fermion', N)

        g = Gf(mesh=mesh, target_shape=[])

        # Fill with arbitrary values
        for mp in mesh:
            g[mp] = mp.value ** 2

        # Interpolation at mesh points should be exact
        for mp in mesh:
            exact = mp.value ** 2
            interp = np.real(g(mp.value))
            self.assertAlmostEqual(exact, interp, places=12)

    def test_interpolation_convergence(self):
        """Test exponential convergence with increasing N"""
        beta = 10.0
        omega = 2.0

        errors = []
        for N in [10, 15, 20, 25, 30]:
            mesh = MeshChebyshev(beta, 'Fermion', N)
            g = Gf(mesh=mesh, target_shape=[])

            for mp in mesh:
                g[mp] = test_function(mp.value, beta, omega)

            # Test at a few points
            max_err = 0.0
            for i in range(1, 20):
                tau = beta * i / 20.0
                exact = test_function(tau, beta, omega)
                interp = np.real(g(tau))
                max_err = max(max_err, abs(exact - interp))
            errors.append(max_err)

        # Error should decrease
        for i in range(1, len(errors)):
            self.assertLess(errors[i], errors[i - 1])

    def test_hdf5(self):
        """Test HDF5 serialization"""
        beta = 10.0
        N = 20
        mesh = MeshChebyshev(beta, 'Fermion', N)
        g = Gf(mesh=mesh, target_shape=[])

        for mp in mesh:
            g[mp] = mp.value ** 2

        fname = 'test_chebyshev_py.h5'
        with HDFArchive(fname, 'w') as ar:
            ar['g'] = g

        with HDFArchive(fname, 'r') as ar:
            g2 = ar['g']

        self.assertEqual(g.mesh, g2.mesh)
        np.testing.assert_allclose(g.data, g2.data)

        os.remove(fname)

    def test_mesh_equality(self):
        """Test mesh equality comparison"""
        m1 = MeshChebyshev(10, 'Fermion', 20)
        m2 = MeshChebyshev(10, 'Fermion', 20)
        m3 = MeshChebyshev(10, 'Fermion', 15)
        m4 = MeshChebyshev(5, 'Fermion', 20)
        m5 = MeshChebyshev(10, 'Boson', 20)

        self.assertEqual(m1, m2)
        self.assertNotEqual(m1, m3)  # Different N
        self.assertNotEqual(m1, m4)  # Different beta
        self.assertNotEqual(m1, m5)  # Different statistics


if __name__ == '__main__':
    unittest.main()
