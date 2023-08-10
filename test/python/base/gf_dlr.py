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

from triqs.gf import *
from triqs.gf.meshes import MeshDLRImTime
from triqs.gf.meshes import MeshDLRImFreq
from triqs.gf.meshes import MeshDLR

from triqs.utility.comparison_tests import *

def onefermion(tau, eps, beta):
    from math import exp
    return -exp(-eps * tau) / (1 + exp(-beta * eps));

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
            #print(ref, g_t[t].real)
            #print(ref, t, g_c[t].real) # FIXME : SHOULD NOT WORK
            #print(ref, t.value,  g_c(t.value))
            #np.testing.assert_almost_equal(g_t[t], ref)
            np.testing.assert_almost_equal(g_c(t), ref)

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
        Bgdlr = BlockGf(name_list=['1', '2'], block_list=[g, g], make_copies=True)

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

if __name__ == '__main__':
    unittest.main()        
    

