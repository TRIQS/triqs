################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011 by M. Ferrero, O. Parcollet
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
import numpy
import unittest

from h5 import *
from triqs.gf import *
from triqs.utility.comparison_tests import *


class test_gf_init(unittest.TestCase):
    def setUp(self):
        pass

    def test_gf_init_lshift(self):

        h=HDFArchive('gf_init.ref.h5','r')

        g = GfImFreq(indices = ['eg1','eg2'], beta = 50, n_points = 100, name = "egBlock")
        g['eg1','eg1'] << SemiCircular(half_bandwidth = 1)
        g['eg2','eg2'] << SemiCircular(half_bandwidth = 2)

        assert_gfs_are_close(g, h['g1'])

        g << numpy.array([[1,2],[2,3]])

        assert_gfs_are_close(g, h['g2'])

        some_mesh = numpy.arange(-5,5,0.1)
        g = GfReFreq(indices = ['eg1','eg2'], window = (-5, 4.9), n_points = 100, name = "egBlock")

        g['eg1','eg1'] << iOmega_n - 1.0
        g['eg2','eg2'] << iOmega_n + 1.0

        assert_gfs_are_close(g, h['g3'])


    def test_gf_init_fromgf(self):

        g1 = GfImFreq(indices = ['eg1','eg2'], beta = 50, n_points = 100, name = "egBlock")
        g1['eg1','eg1'] << SemiCircular(half_bandwidth = 1)
        g1['eg2','eg2'] << SemiCircular(half_bandwidth = 2)

        g2 = GfImFreq(indices = ['eg1','eg2'], beta = 50, n_points = 100, name = "egBlock")
        g2[:] = g1
        assert_gfs_are_close(g1, g2)

        g3 = GfImFreq(indices = ['eg1','eg2'], beta = 50, n_points = 100, name = "egBlock")
        g3 << g1
        assert_gfs_are_close(g1, g3)

if __name__ == '__main__':
    unittest.main()
