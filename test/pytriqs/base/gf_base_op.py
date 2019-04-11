################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011-2014 by M. Ferrero, O. Parcollet
# Copyright (C) 2018 by Simons Foundation
#   author : N. Wentzell
#
# TRIQS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# TRIQS. If not, see <http://www.gnu.org/licenses/>.
#
################################################################################
from pytriqs.archive import *
from pytriqs.gf import *
from pytriqs.utility.comparison_tests import *

import numpy as np, copy
from numpy import linalg
from math import pi
import unittest

def max_abs(a) :
    return np.amax(np.abs(a))

class test_Gf_Base_Op(unittest.TestCase):

    def setUp(self):

        self.precision = 1.e-6
        self.beta = 50.0

    def test_Base_Op(self):

        iw_mesh = MeshImFreq(beta=self.beta, S = "Fermion", n_max = 1000)

        ga = GfImFreq(mesh=iw_mesh, target_shape=(2,2), name = "a1Block")
        gb = GfImFreq(mesh=iw_mesh, target_shape=(2,2), name = "b1Block")

        G = BlockGf(name_list = ('a','b'), block_list = (ga,gb), make_copies = False)
        G << iOmega_n + 2.0

        def matsu(n) :
            return (2*n+1)*pi/self.beta * 1j

        for ii, g in G :
            N = g.data.shape[0]
            for n in range(N/2) :
                assert_array_close_to_scalar( g.data[n+N/2],  matsu(n) + 2.0)

        # Arithmetic operations
        G2 = G.copy()
        G2 << G * G + 1.5 * G

        # inverse:
        G << inverse(G)

        #  Density:
        dens = G.total_density()
        assert abs(dens - 4.000001283004012) < self.precision, "oops dens =  %s"%dens

        # FT:
        f = lambda g,L : GfImTime(indices = g.indices, beta = self.beta, n_points = L )
        gt = BlockGf(name_block_generator = [ (n,f(g,2001) ) for n,g in G], make_copies=False, name='gt')
        for (i,gtt) in gt : gtt.set_from_fourier(G[i])

        res = np.array([[[  3.14815470e-04,   0.00000000e+00],
                [  0.00000000e+00,   3.14815470e-04]],

               [[ -1.48721028e-04,   0.00000000e+00],
                [  0.00000000e+00,  -1.48721028e-04]],

               [[  7.46732524e-05,   0.00000000e+00],
                [  0.00000000e+00,   7.46732524e-05]]])

        assert_arrays_are_close(gt['a'].data[:3], res, 1.e-3)

        # Matrix operations:
        ga2 = GfImFreq(indices = [1,2,3], beta = self.beta, n_points = 1000, name = "a1Block")
        mat = np.array([[1.0,0.0,1.0],[-1.0,1.0,0.0]], np.complex)

        ga2.from_L_G_R(mat.transpose(),ga,mat)

        res = np.array([[[ 0.99901401-0.03138495j, -0.49950701+0.01569248j,
                  0.49950701-0.01569248j],
                [-0.49950701+0.01569248j,  0.49950701-0.01569248j,  0.00000000+0.j        ],
                [ 0.49950701-0.01569248j,  0.00000000+0.j        ,
                  0.49950701-0.01569248j]],

               [[ 0.99119556-0.09341798j, -0.49559778+0.04670899j,
                  0.49559778-0.04670899j],
                [-0.49559778+0.04670899j,  0.49559778-0.04670899j,  0.00000000+0.j        ],
                [ 0.49559778-0.04670899j,  0.00000000+0.j        ,
                  0.49559778-0.04670899j]],

               [[ 0.97592014-0.15329718j, -0.48796007+0.07664859j,
                  0.48796007-0.07664859j],
                [-0.48796007+0.07664859j,  0.48796007-0.07664859j,  0.00000000+0.j        ],
                [ 0.48796007-0.07664859j,  0.00000000+0.j        ,
                  0.48796007-0.07664859j]]])
        shift = ga2.data.shape[0]/2
        assert_arrays_are_close(ga2.data[shift:shift+3], res)

        # conjugate:
        Gc = G.conjugate()

        res = np.array([[[ 0.49950701+0.01569248j,  0.00000000-0.j        ],
                [-0.00000000-0.j        ,  0.49950701+0.01569248j]],

               [[ 0.49559778+0.04670899j,  0.00000000-0.j        ],
                [-0.00000000-0.j        ,  0.49559778+0.04670899j]],

               [[ 0.48796007+0.07664859j,  0.00000000-0.j        ],
                [-0.00000000-0.j        ,  0.48796007+0.07664859j]]])
        shift = Gc['a'].data.shape[0]/2
        assert_arrays_are_close(Gc['a'].data[shift:shift+3], res)

        # to be continued
        # tranpose

        with HDFArchive('gf_base_op_test.h5','w') as h :
            h['g'] = G
            h['gt'] = gt

        # Check reading out of archive
        with HDFArchive('gf_base_op_test.h5','r') as h :
            assert_block_gfs_are_close(h['g'], G)
            assert_block_gfs_are_close(h['gt'], gt)

        # Pickle (also use in mpi, etc...)
        import pickle

        def check_pickle(g):
            s = pickle.dumps(g)
            g2 = pickle.loads(s)
            assert_block_gfs_are_close(g,g2)

        check_pickle(G)
        check_pickle(gt)

    def test_Mat_Prod(self):

        iw_mesh = MeshImFreq(beta=self.beta, S = "Fermion", n_max = 50)

        Mat = np.matrix([[1, 2], [3, 4]])

        G = Gf(mesh=iw_mesh, target_shape=(2,2), name = "G_iw")
	G << iOmega_n * Mat

	G_exact = G.copy()
	G_exact[0, 0] << Mat[0, 0] * iOmega_n
	G_exact[1, 0] << Mat[1, 0] * iOmega_n
	G_exact[0, 1] << Mat[0, 1] * iOmega_n
	G_exact[1, 1] << Mat[1, 1] * iOmega_n

        assert_gfs_are_close(G, G_exact)
        assert_gfs_are_close(Mat * G * linalg.inv(Mat), G_exact)

        # ======

        G = Gf(mesh=MeshProduct(iw_mesh, iw_mesh), target_shape=(2,2), name = "G_iw_iw")
	for iw1, iw2 in G.mesh:
	    G[iw1, iw2] = np.identity(2) / (iw1 + 2.0 * iw2 + 4.0)

	G_exact = G.copy()
	for iw1, iw2 in G.mesh:
	    G_exact[iw1, iw2][0, 0] = Mat[0, 0] / (iw1 + 2.0 * iw2 + 4.0)
	    G_exact[iw1, iw2][1, 0] = Mat[1, 0] / (iw1 + 2.0 * iw2 + 4.0)
	    G_exact[iw1, iw2][0, 1] = Mat[0, 1] / (iw1 + 2.0 * iw2 + 4.0)
	    G_exact[iw1, iw2][1, 1] = Mat[1, 1] / (iw1 + 2.0 * iw2 + 4.0)

        assert_gfs_are_close(G * Mat, G_exact)
        assert_gfs_are_close(Mat * G, G_exact)
        G *= Mat
        assert_gfs_are_close(G, G_exact)
        assert_gfs_are_close(Mat * G * linalg.inv(Mat), G_exact)

if __name__ == '__main__':
    unittest.main()
