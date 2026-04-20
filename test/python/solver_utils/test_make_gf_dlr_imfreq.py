# Copyright (c) 2024-2026 Simons Foundation
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

from triqs.gf import (
    Gf, BlockGf, MeshImFreq, MeshImTime, MeshDLRImFreq,
    iOmega_n, inverse, make_gf_dlr, make_gf_imfreq,
)
from triqs.solver_utils import make_gf_dlr_imfreq


BETA = 10.0
EPS = 1e-10
N_IW = 400


def _scalar_pole_gf(e=1.42, beta=BETA, n_iw=N_IW):
    iw_mesh = MeshImFreq(beta, 'Fermion', n_iw)
    g = Gf(mesh=iw_mesh, target_shape=[])
    g << inverse(iOmega_n - e)
    return g


def _matrix_pole_gf(poles=(1.42, -0.7), beta=BETA, n_iw=N_IW):
    iw_mesh = MeshImFreq(beta, 'Fermion', n_iw)
    g = Gf(mesh=iw_mesh, target_shape=[len(poles), len(poles)])
    for i, e in enumerate(poles):
        g[i, i] << inverse(iOmega_n - e)
    return g


def _round_trip_imfreq(g_dlr_iw, n_iw):
    """Reconstruct a regular ImFreq Gf from a DLR-sampled Gf."""
    return make_gf_imfreq(make_gf_dlr(g_dlr_iw), n_iw)


class TestMakeGfDlrImfreq(unittest.TestCase):

    def test_scalar_explicit_w_max(self):
        g_iw = _scalar_pole_gf()
        g_dlr_iw = make_gf_dlr_imfreq(g_iw, w_max=10.0, eps=EPS)

        self.assertIsInstance(g_dlr_iw.mesh, MeshDLRImFreq)
        self.assertEqual(g_dlr_iw.mesh.w_max, 10.0)
        self.assertEqual(g_dlr_iw.mesh.eps, EPS)

        g_rec = _round_trip_imfreq(g_dlr_iw, N_IW)
        np.testing.assert_allclose(g_rec.data, g_iw.data, atol=1e-8)

    def test_matrix_target(self):
        g_iw = _matrix_pole_gf()
        g_dlr_iw = make_gf_dlr_imfreq(g_iw, w_max=10.0, eps=EPS)

        self.assertEqual(g_dlr_iw.target_shape, g_iw.target_shape)

        g_rec = _round_trip_imfreq(g_dlr_iw, N_IW)
        np.testing.assert_allclose(g_rec.data, g_iw.data, atol=1e-8)

    def test_blockgf_explicit_w_max(self):
        g_iw = _scalar_pole_gf(e=1.42)
        g_iw2 = _scalar_pole_gf(e=-0.7)
        Bg_iw = BlockGf(name_list=['up', 'dn'], block_list=[g_iw, g_iw2], make_copies=True)

        Bg_dlr_iw = make_gf_dlr_imfreq(Bg_iw, w_max=10.0, eps=EPS)

        self.assertIsInstance(Bg_dlr_iw, BlockGf)
        self.assertEqual(list(Bg_dlr_iw.indices), ['up', 'dn'])
        self.assertEqual(Bg_dlr_iw.mesh.w_max, 10.0)

        for name, g_bl in Bg_iw:
            g_rec = _round_trip_imfreq(Bg_dlr_iw[name], N_IW)
            np.testing.assert_allclose(g_rec.data, g_bl.data, atol=1e-8)

    def test_auto_w_max_scalar(self):
        g_iw = _scalar_pole_gf()
        g_dlr_iw = make_gf_dlr_imfreq(g_iw, eps=EPS)

        # The auto-selected w_max must bound the input pole (|e|=1.42).
        self.assertGreater(g_dlr_iw.mesh.w_max, 1.42)
        self.assertLess(g_dlr_iw.mesh.w_max, 200.0)

        g_rec = _round_trip_imfreq(g_dlr_iw, N_IW)
        np.testing.assert_allclose(g_rec.data, g_iw.data, atol=EPS * 10)

    def test_auto_w_max_blockgf_uses_max_over_blocks(self):
        # Block with a small pole should not force the shared w_max too low:
        # the selected w_max must accommodate the largest-pole block.
        g_small = _scalar_pole_gf(e=0.2)
        g_large = _scalar_pole_gf(e=5.0)
        Bg_iw = BlockGf(name_list=['s', 'l'], block_list=[g_small, g_large], make_copies=True)

        Bg_dlr_iw = make_gf_dlr_imfreq(Bg_iw, eps=EPS)

        self.assertGreater(Bg_dlr_iw.mesh.w_max, 5.0)
        for name, g_bl in Bg_iw:
            g_rec = _round_trip_imfreq(Bg_dlr_iw[name], N_IW)
            np.testing.assert_allclose(g_rec.data, g_bl.data, atol=EPS * 10)

    def test_returned_mesh_w_max_matches_request(self):
        g_iw = _scalar_pole_gf()
        for w_max in (5.0, 20.0, 50.0):
            g_dlr_iw = make_gf_dlr_imfreq(g_iw, w_max=w_max, eps=EPS)
            self.assertEqual(g_dlr_iw.mesh.w_max, w_max)

    def test_invalid_input_type_raises(self):
        with self.assertRaises(TypeError):
            make_gf_dlr_imfreq(42)
        with self.assertRaises(TypeError):
            make_gf_dlr_imfreq(np.zeros(5))

    def test_wrong_input_mesh_raises(self):
        tau_mesh = MeshImTime(BETA, 'Fermion', 100)
        g_tau = Gf(mesh=tau_mesh, target_shape=[])
        with self.assertRaises(AssertionError):
            make_gf_dlr_imfreq(g_tau, w_max=10.0, eps=EPS)

    def test_w_max_search_exceeds_bound_raises(self):
        g_iw = _scalar_pole_gf()
        # Starting above the allowed maximum ensures the loop never enters
        # and the RuntimeError is raised deterministically.
        with self.assertRaises(RuntimeError):
            make_gf_dlr_imfreq(g_iw, eps=EPS, w_max_init=1.0, w_max_max=0.5)

    def test_dlr_mesh_exceeds_imfreq_range_raises(self):
        # Tiny n_iw combined with a large w_max / tight eps forces DLR
        # indices beyond the input Matsubara range.
        iw_mesh = MeshImFreq(BETA, 'Fermion', 5)
        g_iw = Gf(mesh=iw_mesh, target_shape=[])
        g_iw << inverse(iOmega_n - 1.0)
        with self.assertRaises(ValueError):
            make_gf_dlr_imfreq(g_iw, w_max=100.0, eps=1e-10)


if __name__ == '__main__':
    unittest.main()
