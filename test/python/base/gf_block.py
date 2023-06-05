# Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2022 Simons Foundation
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
# Authors: Jonathan Karp, Nils Wentzell

from triqs.gf import *
from triqs.utility.comparison_tests import *
from h5 import *

from itertools import product
import unittest

class test_Gf_Block(unittest.TestCase):

    def setUp(self):

        beta = 1.0
        self.iw_mesh = MeshImFreq(beta=beta, S = "Fermion", n_iw = 5)

    def test_Construct(self):

        G1 = Gf(mesh=self.iw_mesh, target_shape=(2,2))
        G2 = G1.copy()
        G3 = G1.copy()
        
        G1 << inverse(iOmega_n + 2.)
        G2 << inverse(iOmega_n - 2.)
        
        G_vec = [G1, G2, G3]
        
        B1 = BlockGf(name_list=['1', '2', '3'], block_list=G_vec)
        B2 = B1.copy()
        
        gf_struct = [('1',2), ('2',2), ('3',2)]
        B3 = BlockGf(mesh=self.iw_mesh, gf_struct=gf_struct)

        B3['1'] << inverse(iOmega_n + 2.)
        B3['2'] << inverse(iOmega_n - 2.)

        assert_block_gfs_are_close(B1, B2)
        assert_block_gfs_are_close(B1, B3)

        # Scalar-valued blocks
        gf_struct = [('1',0), ('2',0), ('3',0)]
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

    def test_mult(self):

        G1 = Gf(mesh=self.iw_mesh, target_shape=(1,1))
        G2 = G1.copy()
        G3 = Gf(mesh=self.iw_mesh, target_shape=(1,2))
        G4 = Gf(mesh=self.iw_mesh, target_shape=(1,3))

        G1 << inverse(2 + iOmega_n)
        G2 << inverse(2 - iOmega_n)
        for i in range(2):
            G3[0,i] << inverse(iOmega_n + 2 + i)
        for i in range(3):
            G4[0,i] << inverse(iOmega_n + 2 + i)

        B1 = BlockGf(name_list=['0', '1'], block_list=[G1, G2])
        B2 = BlockGf(name_list=['0', '1'], block_list=[G3, G4])
        B3 = B1 * B2
        B4 = BlockGf(name_list=['0', '1'], block_list=[G1*G3, G2*G4])

        assert_block_gfs_are_close(B3, B4)

    def test_block2_mult(self):

        G1 = Gf(mesh=self.iw_mesh, target_shape=(2,2))
        G2 = G1.copy()
        G3 = G1.copy()
        G4 = G1.copy()

        G1 << inverse(2 + iOmega_n)
        G2 << inverse(2 - iOmega_n)
        G1 << inverse(3 + iOmega_n)
        G2 << inverse(3 - iOmega_n)

        B1 = Block2Gf(name_list1=['0', '1'], name_list2=['0', '1'], block_list=[[G1, G2], [G2, G1]])
        B2 = Block2Gf(name_list1=['0', '1'], name_list2=['0', '1'], block_list=[[G3, G4], [G3, G4]])
        B3 = B1 * B2
        B4 = Block2Gf(name_list1=['0', '1'], name_list2=['0', '1'], block_list=[[G1*G3, G2*G4], [G2*G3, G1*G4]])

        assert_block2_gfs_are_close(B3, B4)

if __name__ == '__main__':
    unittest.main()
