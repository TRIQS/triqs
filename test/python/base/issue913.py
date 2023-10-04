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
# Authors: Nils Wentzell, Yann in't Veld

import unittest

from triqs.gf import *
from triqs.lattice.lattice_tools import BrillouinZone, BravaisLattice

from triqs.utility.comparison_tests import *

class test_issue913(unittest.TestCase):

    def test_dlr_slice(self):

        bl = BravaisLattice(units=[(1,0,0),(0,1,0)])
        bz = BrillouinZone(bl)
        kmesh = MeshBrZone(bz, [2,2,1])
        
        wmeshDLR_f = MeshDLRImFreq(beta = 100.0, statistic = 'Fermion', w_max = 30.0, eps = 1e-10)
        
        g_Dwk = Gf(mesh=MeshProduct(wmeshDLR_f, kmesh), target_shape=[2,2])
        for k in kmesh:
            g_Dwk[:,k] << inverse(iOmega_n + 0.5)
        
        nw_equidist = 50
        g_c = make_gf_dlr(g_Dwk[:,Idx(0,0,0)])
        g_slice_w = make_gf_imfreq(g_c, nw_equidist)
        
        g_Dw = Gf(mesh=wmeshDLR_f, target_shape=g_Dwk.target_shape)
        g_Dw.data[:] = g_Dwk.data[:,0,...]
        g_c = make_gf_dlr(g_Dw)
        g_w = make_gf_imfreq(g_c, nw_equidist)

        assert_gfs_are_close(g_slice_w, g_w)

if __name__ == '__main__':
    unittest.main()
