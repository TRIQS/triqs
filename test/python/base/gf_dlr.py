# Copyright (c) 2023 Simons Foundation
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
# Authors: Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell

import unittest

import numpy as np
from scipy import integrate
from triqs.gf import Gf, make_gf_dlr, make_gf_dlr_imtime, make_gf_dlr_imfreq, iOmega_n, inverse, density, BlockGf, fit_gf_dlr
from triqs.gf.meshes import MeshDLR, MeshDLRImFreq, MeshDLRImTime, MeshImFreq, MeshImTime
from triqs.utility.comparison_tests import assert_gfs_are_close, assert_block_gfs_are_close


def onefermion(tau, omega, beta):
    from math import exp
    return -exp(-omega * tau) / (1 + exp(-beta * omega))

class test_dlr_mesh(unittest.TestCase):


    def test_dlr_meshes(self):

        beta, eps, w_max = 1.337, 1e-9, 100.

        MeshTypes = [MeshDLRImTime, MeshDLRImFreq, MeshDLR]

        for MeshType in MeshTypes:

            m = MeshType(beta, 'Fermion', w_max, eps)

            assert( m.beta == beta )
            assert( m.statistic == 'Fermion' )
            assert( m.eps == eps )
            assert( m.w_max == w_max )

            mps = np.array([ p.value for p in m ])

            assert( len(mps) == len(m) )


    def test_dlr_gfs_conversion(self):

        e = 1.42
        beta, eps, w_max = 1.337, 1e-9, 100.

        wmesh = MeshDLRImFreq(beta, 'Fermion', w_max , eps)

        g_w = Gf(mesh=wmesh, target_shape=[])
        g_w << inverse(iOmega_n - e)

        g_w_2 = Gf(mesh=wmesh, target_shape=[])
        for w in wmesh:
            g_w_2[w] = 1/(w - e)

        np.testing.assert_array_almost_equal(g_w.data, g_w_2.data)

        g_c = make_gf_dlr(g_w)

        g_t = make_gf_dlr_imtime(g_c)

        for t in g_t.mesh:
            ref = -np.exp(-e * t.value) / ( 1 + np.exp(-e * beta) )
            np.testing.assert_almost_equal(g_t[t], ref)
            np.testing.assert_almost_equal(g_c(t), ref)
            np.testing.assert_almost_equal(g_c(t.value), ref)

        g_w_new = make_gf_dlr_imfreq(g_c)

        for w in g_w_new.mesh:
            np.testing.assert_almost_equal(g_w[w], g_w_new[w])

        # BlockGf test
        Bg_w = BlockGf(block_list=[g_w, g_w], make_copies=True)
        Bg_c = make_gf_dlr(Bg_w)
        Bg_t = make_gf_dlr_imtime(Bg_c)
        for t in g_t.mesh:
            ref = -np.exp(-e * t.value) / ( 1 + np.exp(-e * beta) )
            np.testing.assert_almost_equal(Bg_c['0'](t), ref)

        Bg_w_new = make_gf_dlr_imfreq(Bg_c)
        assert_block_gfs_are_close(Bg_w, Bg_w_new)

    def test_dlr_gfs_density(self):

        e = 1.42
        beta, eps, w_max = 1.337, 1e-9, 100.

        wmesh = MeshDLRImFreq(beta, 'Fermion', w_max, eps)
        g_w = Gf(mesh=wmesh, target_shape=[])

        for w in wmesh: g_w[w] = 1/(w - e)

        g_c = make_gf_dlr(g_w)

        ref = np.exp(-e * beta) / (1 + np.exp(-e * beta))
        np.testing.assert_almost_equal(density(g_c), ref)

        # block gf test
        Bgw = BlockGf(block_list=[g_c, g_c], make_copies=True)
        for block, den in Bgw.density().items():
            np.testing.assert_almost_equal(den, ref)

        # g_w and g_t test
        np.testing.assert_almost_equal(g_w.density(), ref)
        g_t = make_gf_dlr_imtime(g_c)
        np.testing.assert_almost_equal(g_t.density(), ref)


    def test_dlr_gfs_imfreq_interp(self):

        beta, eps, w_max = 1.337, 1e-12, 10.
        m = MeshDLR(beta, 'Fermion', w_max, eps)

        rf = np.array([ p.value for p in m ])

        g = Gf(mesh=m, target_shape=[])
        g.data[:] = np.random.randn(len(m))

        n_iw = 10
        iw_mesh = MeshImFreq(beta, 'Fermion', n_iw)
        for iw in iw_mesh:
            ref = np.sum(g.data / ( complex(iw) - rf/beta ) )
            np.testing.assert_almost_equal(g(iw), ref)


    def test_dlr_gfs_imtime_interp(self):

        beta, eps, w_max = 1.337, 1e-12, 10.
        m = MeshDLR(beta, 'Fermion', w_max, eps)

        rf = np.array([ p.value for p in m ])

        g = Gf(mesh=m, target_shape=[])
        g.data[:] = np.random.randn(len(m))

        # block gf test
        Bgdlr = BlockGf(block_list=[g, g], make_copies=True)

        for tau in np.linspace(0, beta, num=10):
            ref = np.sum(g.data * -np.exp(-rf * tau/beta) / ( 1 + np.exp(-rf) ))
            np.testing.assert_almost_equal(g(tau), ref)
            for block, gdlr in Bgdlr:
                np.testing.assert_almost_equal(gdlr(tau), ref)


    def test_dlr_gfs_imtime_fit(self):

        beta, eps, w_max = 1.337, 1e-10, 10.
        n_tau = 1001
        omega = 1.337

        gtau = Gf(mesh=MeshImTime(beta, 'Fermion', n_tau), target_shape=[])
        for tau in gtau.mesh:
            gtau[tau] = onefermion(tau, omega, eps)

        gc = fit_gf_dlr(gtau, w_max, eps)
        gt = make_gf_dlr_imtime(gc)

        gt2 = gt.copy()
        for tau in gt2.mesh:
            gt2[tau] = onefermion(tau, omega, eps)

        assert_gfs_are_close(gt, gt2)

    def test_dlr_bug_segfault(self):
        dlr_iw_mesh = MeshDLRImFreq(beta=5.0, statistic='Fermion', w_max=1.0, eps=1e-14)

    def test_dlr_basic_op(self):
        beta, eps, w_max = 46.2, 1e-12, 10.
        m = MeshDLRImFreq(beta, 'Fermion', w_max, eps)

        # scalar gf test
        g = Gf(mesh=m, target_shape=[])
        g += 4.
        g = g*2

        for iwn in g.mesh:
            assert g[iwn] == 8.

        # array gf test
        g = Gf(mesh=m, target_shape=[2,2])
        g +=4.
        g += np.array([[0.,-1.],[0.,0.]])
        g -= np.eye(2)

        for iwn in g.mesh:
            assert np.allclose(g[iwn],np.array([[3.,-1.],[0.,3.]]))

    def test_dlr_symmetrized(self):

        beta = 20
        tau_mesh = MeshDLRImTime(beta = beta, statistic = 'Boson', w_max = 2.0, eps = 1e-14, symmetrize = True)

        tau_values = np.fromiter(tau_mesh.values(), dtype=float)
        assert(np.allclose(tau_values, beta - tau_values[::-1]))

        gtau = Gf(mesh = tau_mesh, target_shape = [])
        for tau in gtau.mesh:
            gtau[tau] = onefermion(tau, omega = 1.1, beta = beta)

        gdlr = make_gf_dlr(gtau)
        giw  = make_gf_dlr_imfreq(gdlr)

        iw_values = np.fromiter(giw.mesh.values(), dtype=complex)
        assert(np.allclose(iw_values, -iw_values[::-1]))
        assert(len(iw_values) % 2 == 1) # odd number of frequencies

        assert_gfs_are_close(make_gf_dlr_imtime(make_gf_dlr(giw)), gtau)

    def test_dlr_l2_norm(self):

        beta, eps, w_max = 40.1, 1e-10, 5.
        pole = 0.375
        mesh_dlr_iw = MeshDLRImFreq(beta, 'Fermion', w_max, eps)

        g_dlr_iw = Gf(mesh=mesh_dlr_iw, target_shape=[1,1])
        g_dlr_iw << inverse(iOmega_n - pole)
        g_dlr = make_gf_dlr(g_dlr_iw)
        g_dlr_tau = make_gf_dlr_imtime(g_dlr)

        # create simply one fermion Gf for one pole in tau
        mesh_values = np.linspace(0, beta, 10001)
        G_analytic =-np.exp(-mesh_values[:, None] * pole + beta * (pole < 0.0) * pole) / (1. + np.exp(-beta * np.abs(pole)))
        # calc G^2 = 1/beta \int_0^\beta d\tau G(\tau)^2
        Gsq = integrate.trapezoid(G_analytic[:,0]*G_analytic[:,0],x=mesh_values)/beta
        ref = np.sqrt(Gsq)

        # test all dlr mesh version
        assert( np.allclose(g_dlr.tau_L2_norm(), ref) )
        assert( np.allclose(g_dlr_iw.tau_L2_norm(), ref) )
        assert( np.allclose(g_dlr_tau.tau_L2_norm(), ref) )

        # test rank 2 version
        g_dlr_iw = Gf(mesh=mesh_dlr_iw, target_shape=[2,2])
        g_dlr_iw << inverse(iOmega_n - pole)

        ref_mat = np.eye(2) * ref
        assert( np.allclose(g_dlr_iw.tau_L2_norm(), ref_mat) )


if __name__ == '__main__':
    unittest.main()
