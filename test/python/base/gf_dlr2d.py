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

from triqs.gf import Gf
from triqs.gf.meshes import MeshDLR2D, MeshDLR2DImFreq


class test_dlr2d_mesh(unittest.TestCase):

    def test_dlr2d_mesh_basics(self):
        """Test basic MeshDLR2D construction and properties"""
        beta = 10.0
        w_max = 1.0
        eps = 1e-10

        mesh = MeshDLR2D(beta, w_max, eps, 'PH')

        self.assertEqual(mesh.beta, beta)
        self.assertEqual(mesh.w_max, w_max)
        self.assertEqual(mesh.eps, eps)
        self.assertEqual(mesh.statistic, 'Fermion')
        self.assertEqual(mesh.compressgrid, False)
        self.assertEqual(mesh.compressbasis, True)
        self.assertGreater(len(mesh), 0)
        self.assertGreater(mesh.rank, 0)

    def test_dlr2d_imfreq_mesh_basics(self):
        """Test basic MeshDLR2DImFreq construction and properties"""
        beta = 10.0
        w_max = 1.0
        eps = 1e-10

        mesh = MeshDLR2DImFreq(beta, w_max, eps, 'PH')

        self.assertEqual(mesh.beta, beta)
        self.assertEqual(mesh.w_max, w_max)
        self.assertEqual(mesh.eps, eps)
        self.assertEqual(mesh.statistic, 'Fermion')
        self.assertEqual(mesh.compressgrid, False)
        self.assertEqual(mesh.compressbasis, True)
        self.assertGreater(len(mesh), 0)
        self.assertGreater(mesh.rank, 0)

    def test_dlr2d_compression_options(self):
        """Test different compression settings"""
        beta = 10.0
        w_max = 1.0
        eps = 1e-10

        # Default compression settings
        m1 = MeshDLR2D(beta, w_max, eps, 'PH', False, True)
        self.assertEqual(m1.compressgrid, False)
        self.assertEqual(m1.compressbasis, True)

        # No compression
        m2 = MeshDLR2D(beta, w_max, eps, 'PH', False, False)
        self.assertEqual(m2.compressgrid, False)
        self.assertEqual(m2.compressbasis, False)

        # Different compression should give different mesh sizes
        self.assertNotEqual(m1, m2)

    def test_dlr2d_mesh_iteration(self):
        """Test iteration over MeshDLR2D"""
        mesh = MeshDLR2D(10.0, 1.0, 1e-10, 'PH')

        count = 0
        for mp in mesh:
            count += 1
            # Mesh point should have a value (pair of DLR frequencies)
            val = mp.value
            self.assertIsInstance(val, tuple)
            self.assertEqual(len(val), 2)

        self.assertEqual(count, len(mesh))

    def test_dlr2d_imfreq_mesh_iteration(self):
        """Test iteration over MeshDLR2DImFreq"""
        mesh = MeshDLR2DImFreq(10.0, 1.0, 1e-10, 'PH')

        count = 0
        for mp in mesh:
            count += 1
            # Mesh point should have a value (pair of Matsubara frequencies)
            val = mp.value
            self.assertIsInstance(val, tuple)
            self.assertEqual(len(val), 2)

        self.assertEqual(count, len(mesh))

    def test_dlr2d_mesh_equality(self):
        """Test MeshDLR2D equality comparison"""
        m1 = MeshDLR2D(10.0, 1.0, 1e-10, 'PH')
        m2 = MeshDLR2D(10.0, 1.0, 1e-10, 'PH')
        m3 = MeshDLR2D(10.0, 2.0, 1e-10, 'PH')  # Different w_max
        m4 = MeshDLR2D(5.0, 1.0, 1e-10, 'PH')   # Different beta

        self.assertEqual(m1, m2)
        self.assertNotEqual(m1, m3)
        self.assertNotEqual(m1, m4)

    def test_dlr2d_imfreq_mesh_equality(self):
        """Test MeshDLR2DImFreq equality comparison"""
        m1 = MeshDLR2DImFreq(10.0, 1.0, 1e-10, 'PH')
        m2 = MeshDLR2DImFreq(10.0, 1.0, 1e-10, 'PH')
        m3 = MeshDLR2DImFreq(10.0, 2.0, 1e-10, 'PH')  # Different w_max
        m4 = MeshDLR2DImFreq(5.0, 1.0, 1e-10, 'PH')   # Different beta

        self.assertEqual(m1, m2)
        self.assertNotEqual(m1, m3)
        self.assertNotEqual(m1, m4)

    def test_dlr2d_mesh_conversions(self):
        """Test conversion between MeshDLR2D and MeshDLR2DImFreq"""
        m_dlr2d = MeshDLR2D(10.0, 1.0, 1e-10, 'PH')
        m_dlr2d_imfreq = MeshDLR2DImFreq(10.0, 1.0, 1e-10, 'PH')

        # dlr2d -> dlr2d_imfreq
        m_dlr2d_imfreq_from_dlr2d = MeshDLR2DImFreq(m_dlr2d)
        self.assertEqual(m_dlr2d_imfreq, m_dlr2d_imfreq_from_dlr2d)

        # dlr2d_imfreq -> dlr2d
        m_dlr2d_from_imfreq = MeshDLR2D(m_dlr2d_imfreq)
        self.assertEqual(m_dlr2d, m_dlr2d_from_imfreq)

    def test_dlr2d_dlr_rf_property(self):
        """Test access to DLR real frequencies"""
        mesh = MeshDLR2D(10.0, 1.0, 1e-10, 'PH')

        dlr_rf = mesh.dlr_rf
        self.assertEqual(len(dlr_rf), mesh.rank)

        # Same for imfreq mesh
        mesh_iw = MeshDLR2DImFreq(10.0, 1.0, 1e-10, 'PH')
        dlr_rf_iw = mesh_iw.dlr_rf
        self.assertEqual(len(dlr_rf_iw), mesh_iw.rank)

    def test_dlr2d_gf_scalar_valued(self):
        """Test scalar-valued GF on MeshDLR2D"""
        mesh = MeshDLR2D(10.0, 1.0, 1e-10, 'PH')

        g = Gf(mesh=mesh, target_shape=[])
        self.assertEqual(len(g.data), len(mesh))

        # Fill with values
        g.data[:] = np.arange(len(mesh))

        # Check values
        for i, mp in enumerate(mesh):
            self.assertEqual(g[mp], i)

    def test_dlr2d_imfreq_gf_scalar_valued(self):
        """Test scalar-valued GF on MeshDLR2DImFreq"""
        mesh = MeshDLR2DImFreq(10.0, 1.0, 1e-10, 'PH')

        g = Gf(mesh=mesh, target_shape=[])
        self.assertEqual(len(g.data), len(mesh))

        # Fill with values
        g.data[:] = np.arange(len(mesh))

        # Check values
        for i, mp in enumerate(mesh):
            self.assertEqual(g[mp], i)

    def test_dlr2d_gf_matrix_valued(self):
        """Test matrix-valued GF on MeshDLR2D"""
        mesh = MeshDLR2D(10.0, 1.0, 1e-10, 'PH')

        g = Gf(mesh=mesh, target_shape=[2, 2])
        g.data[:] = 0.0

        # Fill diagonal with values
        for i, mp in enumerate(mesh):
            g[mp] = np.eye(2) * i

        # Check values
        for i, mp in enumerate(mesh):
            np.testing.assert_array_equal(g[mp], np.eye(2) * i)

    def test_dlr2d_hdf5(self):
        """Test HDF5 serialization of MeshDLR2D GF"""
        mesh = MeshDLR2D(10.0, 1.0, 1e-10, 'PH')
        g = Gf(mesh=mesh, target_shape=[])
        g.data[:] = np.random.randn(len(mesh))

        fname = 'test_dlr2d_py.h5'
        with HDFArchive(fname, 'w') as ar:
            ar['g'] = g

        with HDFArchive(fname, 'r') as ar:
            g2 = ar['g']

        self.assertEqual(g.mesh, g2.mesh)
        np.testing.assert_allclose(g.data, g2.data)

        os.remove(fname)

    def test_dlr2d_imfreq_hdf5(self):
        """Test HDF5 serialization of MeshDLR2DImFreq GF"""
        mesh = MeshDLR2DImFreq(10.0, 1.0, 1e-10, 'PH')
        g = Gf(mesh=mesh, target_shape=[])
        g.data[:] = np.random.randn(len(mesh)) + 1j * np.random.randn(len(mesh))

        fname = 'test_dlr2d_imfreq_py.h5'
        with HDFArchive(fname, 'w') as ar:
            ar['g'] = g

        with HDFArchive(fname, 'r') as ar:
            g2 = ar['g']

        self.assertEqual(g.mesh, g2.mesh)
        np.testing.assert_allclose(g.data, g2.data)

        os.remove(fname)


    def test_dlr2d_channel_parameter(self):
        """Test channel parameter for MeshDLR2D"""
        # Particle-hole channel
        m1 = MeshDLR2D(10.0, 1.0, 1e-10, 'PH')
        self.assertEqual(m1.channel, 'PH')

        # Explicit particle-hole channel
        m2 = MeshDLR2D(10.0, 1.0, 1e-10, 'PH', False, True)
        self.assertEqual(m2.channel, 'PH')
        self.assertEqual(m1, m2)

        # Particle-particle channel
        m3 = MeshDLR2D(10.0, 1.0, 1e-10, 'PP', False, True)
        self.assertEqual(m3.channel, 'PP')
        self.assertNotEqual(m1, m3)  # Different channel -> different mesh

    def test_dlr2d_imfreq_channel_parameter(self):
        """Test channel parameter for MeshDLR2DImFreq"""
        # Particle-hole channel
        m1 = MeshDLR2DImFreq(10.0, 1.0, 1e-10, 'PH')
        self.assertEqual(m1.channel, 'PH')

        # Particle-particle channel
        m2 = MeshDLR2DImFreq(10.0, 1.0, 1e-10, 'PP', False, True)
        self.assertEqual(m2.channel, 'PP')

    def test_dlr2d_channel_conversion(self):
        """Test channel is preserved in mesh conversions"""
        m_dlr2d = MeshDLR2D(10.0, 1.0, 1e-10, 'PP', False, True)
        self.assertEqual(m_dlr2d.channel, 'PP')

        # Convert to imfreq and back
        m_imfreq = MeshDLR2DImFreq(m_dlr2d)
        self.assertEqual(m_imfreq.channel, 'PP')

        m_dlr2d_back = MeshDLR2D(m_imfreq)
        self.assertEqual(m_dlr2d_back.channel, 'PP')

    def test_dlr2d_gf_evaluation_scalar(self):
        """Test scalar-valued GF evaluation at arbitrary frequency pairs"""
        from triqs.gf import MatsubaraFreq

        mesh = MeshDLR2D(10.0, 1.0, 1e-10, 'PH')
        g = Gf(mesh=mesh, target_shape=[])

        # Fill with some values
        for i, mp in enumerate(mesh):
            g[mp] = complex(i, 0.5 * i)

        # Create Matsubara frequencies for evaluation
        iw0 = MatsubaraFreq(0, mesh.beta, 'Fermion')
        iw1 = MatsubaraFreq(1, mesh.beta, 'Fermion')
        iw5 = MatsubaraFreq(5, mesh.beta, 'Fermion')
        iwm5 = MatsubaraFreq(-5, mesh.beta, 'Fermion')

        # Evaluate at various frequency pairs using tuple syntax: g((iw1, iw2))
        val1 = g((iw0, iw0))
        val2 = g((iw1, iw0))
        val3 = g((iw0, iw1))
        val4 = g((iw5, iwm5))

        # Values should be finite complex numbers
        self.assertTrue(np.isfinite(val1.real))
        self.assertTrue(np.isfinite(val1.imag))
        self.assertTrue(np.isfinite(val2.real))
        self.assertTrue(np.isfinite(val3.real))
        self.assertTrue(np.isfinite(val4.real))

    def test_dlr2d_gf_evaluation_matrix(self):
        """Test matrix-valued GF evaluation at arbitrary frequency pairs"""
        from triqs.gf import MatsubaraFreq

        mesh = MeshDLR2D(10.0, 1.0, 1e-10, 'PH')
        g = Gf(mesh=mesh, target_shape=[2, 2])

        # Fill with some values
        for i, mp in enumerate(mesh):
            g[mp] = np.eye(2) * complex(i, 0.5 * i)

        # Create Matsubara frequencies for evaluation
        iw0 = MatsubaraFreq(0, mesh.beta, 'Fermion')

        # Evaluate at a frequency pair using tuple syntax
        val = g((iw0, iw0))

        # Result should be a 2x2 matrix
        self.assertEqual(val.shape, (2, 2))
        self.assertTrue(np.all(np.isfinite(val)))

    def test_dlr2d_gf_evaluation_zero_coefs(self):
        """Test that zero coefficients give zero evaluation"""
        from triqs.gf import MatsubaraFreq

        mesh = MeshDLR2D(10.0, 1.0, 1e-10, 'PH')
        g = Gf(mesh=mesh, target_shape=[])

        # All zeros
        g.data[:] = 0.0

        # Create Matsubara frequency for evaluation
        iw0 = MatsubaraFreq(0, mesh.beta, 'Fermion')

        # Evaluate should give zero
        val = g((iw0, iw0))
        self.assertAlmostEqual(abs(val), 0.0, places=14)


class test_dlr2d_gf_conversions(unittest.TestCase):

    def test_scalar_roundtrip(self):
        """Test roundtrip conversion: dlr2d -> dlr2d_imfreq -> dlr2d -> dlr2d_imfreq (scalar)

        Start from DLR2D coefficients (which define data in the DLR2D subspace),
        then verify that the imfreq roundtrip preserves the values exactly.
        """
        from triqs.gf import make_gf_dlr2d, make_gf_dlr2d_imfreq

        # Start from DLR2D coefficients (this ensures data is in the DLR2D subspace)
        mesh_dlr2d = MeshDLR2D(10.0, 1.0, 1e-10, 'PH')
        g_dlr2d = Gf(mesh=mesh_dlr2d, target_shape=[])

        # Fill with test coefficients
        for i in range(len(mesh_dlr2d)):
            g_dlr2d.data[i] = complex(0.1 * (i + 1), -0.05 * i)

        # Convert to imfreq
        g_iw = make_gf_dlr2d_imfreq(g_dlr2d)
        mesh_iw = MeshDLR2DImFreq(mesh_dlr2d)
        self.assertEqual(len(g_iw.data), len(mesh_iw))

        # Convert back to coefficients and then to imfreq again
        g_iw_back = make_gf_dlr2d_imfreq(make_gf_dlr2d(g_iw))
        self.assertEqual(g_iw_back.mesh, mesh_iw)

        # Check roundtrip preserves values exactly (data is in DLR2D subspace)
        np.testing.assert_allclose(g_iw.data, g_iw_back.data, rtol=1e-10, atol=1e-10)

    def test_matrix_roundtrip(self):
        """Test roundtrip conversion for matrix-valued GF: dlr2d -> dlr2d_imfreq -> dlr2d -> dlr2d_imfreq"""
        from triqs.gf import make_gf_dlr2d, make_gf_dlr2d_imfreq

        # Start from DLR2D coefficients (this ensures data is in the DLR2D subspace)
        mesh_dlr2d = MeshDLR2D(10.0, 1.0, 1e-10, 'PH')
        g_dlr2d = Gf(mesh=mesh_dlr2d, target_shape=[2, 2])

        # Fill with test coefficients
        for i in range(len(mesh_dlr2d)):
            g_dlr2d.data[i, 0, 0] = complex(0.1 * (i + 1), -0.05 * i)
            g_dlr2d.data[i, 0, 1] = complex(0.02 * i, -0.01 * (i + 1))
            g_dlr2d.data[i, 1, 0] = complex(-0.02 * i, 0.01 * (i + 1))
            g_dlr2d.data[i, 1, 1] = complex(0.05 * (i + 2), -0.03 * i)

        # Convert to imfreq
        g_iw = make_gf_dlr2d_imfreq(g_dlr2d)
        self.assertEqual(g_iw.target_shape, (2, 2))

        # Convert back to coefficients and then to imfreq again
        g_iw_back = make_gf_dlr2d_imfreq(make_gf_dlr2d(g_iw))
        self.assertEqual(g_iw_back.mesh, g_iw.mesh)
        self.assertEqual(g_iw_back.target_shape, (2, 2))

        # Check roundtrip preserves values exactly (data is in DLR2D subspace)
        np.testing.assert_allclose(g_iw.data, g_iw_back.data, rtol=1e-10, atol=1e-10)

    def test_block_gf_roundtrip(self):
        """Test block GF conversion: dlr2d -> dlr2d_imfreq -> dlr2d -> dlr2d_imfreq"""
        from triqs.gf import BlockGf, make_gf_dlr2d, make_gf_dlr2d_imfreq

        # Start from DLR2D coefficients (this ensures data is in the DLR2D subspace)
        mesh_dlr2d = MeshDLR2D(10.0, 1.0, 1e-10, 'PH')
        bg_dlr2d = BlockGf(name_list=['up', 'down'], block_list=[Gf(mesh=mesh_dlr2d, target_shape=[2, 2]),
                                                                  Gf(mesh=mesh_dlr2d, target_shape=[2, 2])])

        # Fill with test coefficients
        for b, (_, g) in enumerate(bg_dlr2d):
            for i in range(len(mesh_dlr2d)):
                g.data[i, 0, 0] = (b + 1) * complex(0.1 * (i + 1), -0.05 * i)
                g.data[i, 0, 1] = complex(0.02 * i, -0.01 * (b + 1) * (i + 1))
                g.data[i, 1, 0] = complex(-0.02 * i, 0.01 * (b + 1) * (i + 1))
                g.data[i, 1, 1] = (b + 1) * complex(0.05 * (i + 2), -0.03 * i)

        # Convert to imfreq
        bg_iw = make_gf_dlr2d_imfreq(bg_dlr2d)
        self.assertEqual(len(list(bg_iw.indices)), 2)

        # Convert back to coefficients and then to imfreq again
        bg_iw_back = make_gf_dlr2d_imfreq(make_gf_dlr2d(bg_iw))
        self.assertEqual(len(list(bg_iw_back.indices)), 2)

        # Check roundtrip preserves values exactly (data is in DLR2D subspace)
        for (name1, g1), (name2, g2) in zip(bg_iw, bg_iw_back):
            self.assertEqual(name1, name2)
            np.testing.assert_allclose(g1.data, g2.data, rtol=1e-10, atol=1e-10)

    def test_zero_coefficients(self):
        """Test that zero coefficients give zero imfreq values"""
        from triqs.gf import make_gf_dlr2d_imfreq

        mesh_dlr2d = MeshDLR2D(10.0, 1.0, 1e-10, 'PH')
        g_dlr2d = Gf(mesh=mesh_dlr2d, target_shape=[])

        # All zeros
        g_dlr2d.data[:] = 0.0

        # Convert to imfreq
        g_iw = make_gf_dlr2d_imfreq(g_dlr2d)

        # All values should be zero
        np.testing.assert_allclose(g_iw.data, 0.0, atol=1e-14)

    def test_different_channels(self):
        """Test different channels: dlr2d -> dlr2d_imfreq -> dlr2d -> dlr2d_imfreq"""
        from triqs.gf import make_gf_dlr2d, make_gf_dlr2d_imfreq

        # Particle-hole channel
        mesh_dlr2d_ph = MeshDLR2D(10.0, 1.0, 1e-10, 'PH', False, True)
        g_dlr2d_ph = Gf(mesh=mesh_dlr2d_ph, target_shape=[])
        for i in range(len(mesh_dlr2d_ph)):
            g_dlr2d_ph.data[i] = complex(0.1 * (i + 1), -0.05 * i)

        g_iw_ph = make_gf_dlr2d_imfreq(g_dlr2d_ph)
        g_iw_ph_back = make_gf_dlr2d_imfreq(make_gf_dlr2d(g_iw_ph))
        np.testing.assert_allclose(g_iw_ph.data, g_iw_ph_back.data, rtol=1e-10, atol=1e-10)

        # Particle-particle channel
        mesh_dlr2d_pp = MeshDLR2D(10.0, 1.0, 1e-10, 'PP', False, True)
        g_dlr2d_pp = Gf(mesh=mesh_dlr2d_pp, target_shape=[])
        for i in range(len(mesh_dlr2d_pp)):
            g_dlr2d_pp.data[i] = complex(0.1 * (i + 1), -0.05 * i)

        g_iw_pp = make_gf_dlr2d_imfreq(g_dlr2d_pp)
        g_iw_pp_back = make_gf_dlr2d_imfreq(make_gf_dlr2d(g_iw_pp))
        np.testing.assert_allclose(g_iw_pp.data, g_iw_pp_back.data, rtol=1e-10, atol=1e-10)


if __name__ == '__main__':
    unittest.main()
