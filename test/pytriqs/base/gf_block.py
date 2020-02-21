from __future__ import unicode_literals
from pytriqs.gf import *
from pytriqs.utility.comparison_tests import *
from pytriqs.archive import *

from itertools import product
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
        
        gf_struct = [ ['1', ['0', '1']], ['2',['0', '1']], ['3',['0', '1']] ]
        B3 = BlockGf(mesh=self.iw_mesh, gf_struct=gf_struct)

        B3['1'] << inverse(iOmega_n + 2.)
        B3['2'] << inverse(iOmega_n - 2.)

        assert_block_gfs_are_close(B1, B2)
        assert_block_gfs_are_close(B1, B3)

        # Scalar-valued blocks
        gf_struct = [ ['1', []], ['2',[]], ['3',[]] ]
        B4 = BlockGf(mesh=self.iw_mesh, gf_struct=gf_struct)

        # Map_block functionality
        B5 = map_block(lambda x: 2*x, B3)
        assert_block_gfs_are_close(B5, 2*B3)

    def test_real_imag(self):

        G1 = Gf(mesh=self.iw_mesh, target_shape=(2,2))
        G2 = G1.copy()

        G1 << inverse(2 + iOmega_n)
        G2 << inverse(2 - iOmega_n)

        B1 = BlockGf(name_list=['0', '1'], block_list=[G1, G2])
        B2 = BlockGf(name_list=['0', '1'], block_list=[G2, G1])

        assert_block_gfs_are_close(-B1.imag, B2.imag)

    def test_block2_real_imag(self):

        G1 = Gf(mesh=self.iw_mesh, target_shape=(2,2))
        G2 = G1.copy()

        G1 << inverse(2 + iOmega_n)
        G2 << inverse(2 - iOmega_n)

        B1 = Block2Gf(name_list1=['0', '1'], name_list2=['0', '1'], block_list=[[G1, G2], [G2, G1]])
        B2 = Block2Gf(name_list1=['0', '1'], name_list2=['0', '1'], block_list=[[G2, G1], [G1, G2]])

        assert_block2_gfs_are_close(-B1.imag, B2.imag)

if __name__ == '__main__':
    unittest.main()
