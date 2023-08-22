# Copyright (c) 2013-2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2017 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020-2023 Simons Foundation
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
# Authors: Michel Ferrero, Olivier Parcollet, Markus Richter, Nils Wentzell

import numpy
import unittest

from h5 import *
from triqs.gf import *
from triqs.utility.comparison_tests import *


class test_gf_init(unittest.TestCase):
    def setUp(self):
        pass

    def test_gf_init_semicircular(self):

        g = GfImFreq(beta = 50, n_points = 100, target_shape = (2,2), name = "egBlock")
        g[0,0] << SemiCircular(half_bandwidth = 1)
        g[1,1] << SemiCircular(half_bandwidth = 2)

        h=HDFArchive('gf_init.ref.h5','r')
        assert_gfs_are_close(g, h['g1'])

        dlr_mesh = MeshDLRImFreq(beta = 50, statistic = 'Fermion', w_max = 10.0, eps = 1e-8)
        gdlr = Gf(mesh = dlr_mesh, target_shape = (2,2))
        gdlr[0,0] << SemiCircular(half_bandwidth = 1)
        gdlr[1,1] << SemiCircular(half_bandwidth = 2)
        assert_gfs_are_close(g, make_gf_imfreq(make_gf_dlr(gdlr), n_iw = 100))

    def test_gf_init_flat(self):

        g = GfImFreq(beta = 50, n_points = 100, target_shape = (2,2), name = "egBlock")
        g[0,0] << Flat(half_bandwidth = 1)
        g[1,1] << Flat(half_bandwidth = 2)

        dlr_mesh = MeshDLRImFreq(beta = 50, statistic = 'Fermion', w_max = 10.0, eps = 1e-8)
        gdlr = Gf(mesh = dlr_mesh, target_shape = (2,2))
        gdlr[0,0] << Flat(half_bandwidth = 1)
        gdlr[1,1] << Flat(half_bandwidth = 2)
        assert_gfs_are_close(g, make_gf_imfreq(make_gf_dlr(gdlr), n_iw = 100))

    def test_gf_init_lshift(self):

        g = GfImFreq(beta = 50, n_points = 100, target_shape = (2,2), name = "egBlock")
        g << numpy.array([[1,2],[2,3]])

        h=HDFArchive('gf_init.ref.h5','r')
        assert_gfs_are_close(g, h['g2'])

        some_mesh = numpy.arange(-5,5,0.1)
        g = GfReFreq(window = (-5, 4.9), n_points = 100, target_shape = (2,2), name = "egBlock")

        g[0,0] << iOmega_n - 1.0
        g[1,1] << iOmega_n + 1.0

        assert_gfs_are_close(g, h['g3'])

    def test_gf_init_fromgf(self):

        g1 = GfImFreq(beta = 50, n_points = 100, target_shape = (2,2), name = "egBlock")
        g1[0,0] << SemiCircular(half_bandwidth = 1)
        g1[1,1] << SemiCircular(half_bandwidth = 2)

        g2 = GfImFreq(beta = 50, n_points = 100, target_shape = (2,2), name = "egBlock")
        g2[:] = g1
        assert_gfs_are_close(g1, g2)

        g3 = GfImFreq(beta = 50, n_points = 100, target_shape = (2,2), name = "egBlock")
        g3 << g1
        assert_gfs_are_close(g1, g3)

        # If meshes are incompatible we should see an AssertionError
        g4 = GfImFreq(beta = 100, n_points = 100, target_shape = (2,2), name = "egBlock")
        with self.assertRaises(AssertionError):
            g4 << g1

    def test_gf_rank1(self):

        beta, n_points = 50, 100
        iw_mesh = MeshImFreq(beta, 'Fermion', n_points)
        g1 = Gf(mesh = iw_mesh, target_shape = [2])
        g1[0] << SemiCircular(half_bandwidth = 1)
        g1[1] << SemiCircular(half_bandwidth = 2)

        g2 = Gf(mesh = iw_mesh, target_shape = [2])
        g2[:] = g1
        assert_gfs_are_close(g1, g2)

        g3 = Gf(mesh = iw_mesh, target_shape = [2])
        g3 << g1
        assert_gfs_are_close(g1, g3)

        g3[1] << g3[0]
        assert_gfs_are_close(g1[0], g3[1])


if __name__ == '__main__':
    unittest.main()
