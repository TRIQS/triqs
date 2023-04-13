
# Copyright (c) 2023-2023 Hugo U. R. Strand
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
# Authors: Hugo U. R. Strand

import unittest

import numpy as np

from triqs.gf import *
from triqs.gf.meshes import MeshDLRImTime
from triqs.gf.meshes import MeshDLRImFreq
from triqs.gf.meshes import MeshDLRCoeffs

from triqs.gf.gf_fnt import dlr_coeffs_from_dlr_imfreq
from triqs.gf.gf_fnt import dlr_coeffs_from_dlr_imtime

from triqs.gf.gf_fnt import dlr_imfreq_from_dlr_coeffs
from triqs.gf.gf_fnt import dlr_imtime_from_dlr_coeffs

class test_dlr_mesh(unittest.TestCase):

    
    def setUp(self):
        pass

    
    def test_dlr_meshes(self):

        beta, eps, lamb = 1.337, 1e-9, 100.

        MeshTypes = [MeshDLRImTime, MeshDLRImFreq, MeshDLRCoeffs]
        
        for MeshType in MeshTypes:
            
            m = MeshType(beta, 'Fermion', lamb, eps)

            assert( m.beta == beta )
            assert( m.statistic == 'Fermion' )
            assert( m.eps == eps )
            assert( m.lamb == lamb )

            mps = np.array([ p.value for p in m ])
        
            assert( len(mps) == m.size )
            assert( len(mps) == len(m) )


    def test_dlr_gfs_conversion(self):

        e = 1.42
        beta, eps, lamb = 1.337, 1e-9, 100.

        wmesh = MeshDLRImFreq(beta, 'Fermion', lamb, eps)

        g_w = Gf(mesh=wmesh, target_shape=[])
        g_w << inverse(iOmega_n + e)
        
        g_w_2 = Gf(mesh=wmesh, target_shape=[])
        for w in wmesh:
            g_w_2[w] = 1/(w + e)

        np.testing.assert_array_almost_equal(g_w.data, g_w_2.data)

        g_c = dlr_coeffs_from_dlr_imfreq(g_w)

        g_t = dlr_imtime_from_dlr_coeffs(g_c)

        for t in g_t.mesh:
            ref = -np.exp(-e * t.value) / ( 1 + np.exp(-e * beta) )
            #print(ref, g_t[t].real)
            np.testing.assert_almost_equal(g_t[t], ref)

        g_w_new = dlr_imfreq_from_dlr_coeffs(g_c)

        for w in g_w_new.mesh:
            np.testing.assert_almost_equal(g_w[w], g_w_new[w])
            
    
    def test_dlr_gfs_density(self):

        e = 1.42
        beta, eps, lamb = 1.337, 1e-9, 100.

        wmesh = MeshDLRImFreq(beta, 'Fermion', lamb, eps)
        g_w = Gf(mesh=wmesh, target_shape=[])

        for w in wmesh: g_w[w] = 1/(w + e)

        g_c = dlr_coeffs_from_dlr_imfreq(g_w)

        ref = np.exp(-e * beta) / (1 + np.exp(-e * beta))
        np.testing.assert_almost_equal(density(g_c), ref)


    def test_dlr_gfs_imfreq_interp(self):

        beta, eps, lamb = 1.337, 1e-12, 10.
        m = MeshDLRCoeffs(beta, 'Fermion', lamb, eps)

        rf = np.array([ p.value for p in m ])
        
        g = Gf(mesh=m, target_shape=[])
        g.data[:] = np.random.randn(len(m))

        N = 10
        for n in np.arange(-N, N):
            iw = 1.j * 2 * np.pi / beta * (2*n + 1)
            ref = - np.sum(g.data / ( iw + rf/beta ) )
            #print(g(iw), ref)
            # This breaks because we currently can not have
            # two call proxies in the python layer
            # see the call operators in python/triqs/gf/wrapped_aux_desc.py
            np.testing.assert_almost_equal(g(iw), ref)

    
    def test_dlr_gfs_imtime_interp(self):

        beta, eps, lamb = 1.337, 1e-12, 10.
        m = MeshDLRCoeffs(beta, 'Fermion', lamb, eps)

        rf = np.array([ p.value for p in m ])
        
        g = Gf(mesh=m, target_shape=[])
        g.data[:] = np.random.randn(len(m))

        for tau in np.linspace(0, beta, num=10):
            ref = np.sum(g.data * np.exp(-rf * tau/beta) / ( 1 + np.exp(-rf) ))
            #print(g(tau), ref)
            np.testing.assert_almost_equal(g(tau), ref)

        

if __name__ == '__main__':
    unittest.main()        
    

