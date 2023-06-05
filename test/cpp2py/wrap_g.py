# Copyright (c) 2014-2015 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2014-2015 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020 Simons Foundation
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
# Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

from test_g import *
from triqs.utility.comparison_tests import *
from h5 import *

import unittest

class test_wrap_gf(unittest.TestCase):

    def test_gf(self):
        g = make_gf(2.0)
        pass_gf(g)

        with HDFArchive('test_wrap_gf.h5', 'r') as a:
            assert_gfs_are_close(a['gout'], a['gin'])


    def test_block_gf(self):
        bg = make_bgf(2.0)
        pass_bgf(bg)

        with HDFArchive('test_wrap_gf.h5', 'r') as a:
            assert_block_gfs_are_close(a['bgout'], a['bgin'])

    def test_pair_gf(self):
        g1, g2 = make_pair_gf(2.0)
        pass_two_gf(g1, g2)

        with HDFArchive('test_wrap_gf.h5', 'r') as a:
            assert_gfs_are_close(a['g1out'], a['g1in'])
            assert_gfs_are_close(a['g2out'], a['g2in'])

if __name__ == '__main__':
    unittest.main()
