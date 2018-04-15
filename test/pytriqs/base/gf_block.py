from pytriqs.gf import *
from pytriqs.utility.comparison_tests import *
from pytriqs.archive import *

from itertools import izip, product
import unittest

class test_Gf_Block(unittest.TestCase):

    def setUp(self):

        beta = 1.0
        self.iw_mesh = MeshImFreq(beta=beta, S = "Fermion", n_max = 5)

    def test_Construct(self):

        G1 = Gf(mesh=self.iw_mesh, target_shape=(2,2))
        G2 = G1.copy()
        G3 = G1.copy()
        
        G1 << inverse(iOmega_n + 2.)
        G2 << inverse(iOmega_n - 2.)
        
        G_vec = [G1, G2, G3]
        
        B1 = BlockGf(name_list=['0', '1', '2'], block_list=G_vec)
        B2 = B1.copy()
        
        gf_struct = [ ['1', [0, 1]], ['2',[0, 1]], ['3',[0, 1]] ]
        B3 = BlockGf(mesh=self.iw_mesh, gf_struct=gf_struct)

        B3['1'] << inverse(iOmega_n + 2.)
        B3['2'] << inverse(iOmega_n - 2.)

        assert_block_gfs_are_close(B1, B2)
        assert_block_gfs_are_close(B1, B3)

if __name__ == '__main__':
    unittest.main()
