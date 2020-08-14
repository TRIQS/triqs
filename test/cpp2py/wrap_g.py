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
