# Copyright (c) 2013-2014 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2014 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2019-2020 Simons Foundation
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
# Authors: Olivier Parcollet, Nils Wentzell

import unittest
from math import exp

from triqs.gfs import Gf, BlockGf, MeshImFreq, MeshImTime, MeshDLRImFreq, iOmega_n, inverse
from triqs.utility.comparison_tests import assert_gfs_are_close, assert_block_gfs_are_close
import triqs.utility.mpi as mpi

class test_bcast(unittest.TestCase):

    def test_bcast_imfreq(self):

        iw_mesh = MeshImFreq(beta=10, statistic='Fermion', n_iw=100)
        g_iw = Gf(mesh = iw_mesh, target_shape = [2, 2])
        g_iw << inverse(iOmega_n + 0.5)
        g_iw_bcast = mpi.bcast(g_iw)
        assert_gfs_are_close(g_iw, g_iw_bcast)

    def test_bcast_imtime(self):

        tau_mesh = MeshImTime(beta=10, statistic='Fermion', n_tau=100)
        g_tau = Gf(mesh = tau_mesh, target_shape = [2, 2])
        def onefermion(tau, eps, beta): return -exp(-eps * tau) / (1.0 + exp(-beta * eps))
        for tau in g_tau.mesh:
            g_tau[tau] = onefermion(tau, 0.1, 10)
        g_tau_bcast = mpi.bcast(g_tau)
        assert_gfs_are_close(g_tau, g_tau_bcast)

    def test_bcast_dlr_iw(self):

        dlr_iw_mesh = MeshDLRImFreq(beta=10, statistic='Fermion', w_max=5.0, eps=1e-10)
        g_dlr_iw = Gf(mesh = dlr_iw_mesh, target_shape = [2, 2])
        g_dlr_iw << inverse(iOmega_n + 0.5)
        g_dlr_iw_bcast = mpi.bcast(g_dlr_iw)
        assert_gfs_are_close(g_dlr_iw, g_dlr_iw_bcast)

    def test_bcast_block(self):

        iw_mesh = MeshImFreq(beta=10, statistic='Fermion', n_iw=100)
        g_iw = Gf(mesh = iw_mesh, target_shape = [2, 2])
        g_iw << inverse(iOmega_n + 0.5)
        g_bl = BlockGf(name_list = ('bl1','bl2'), block_list = (g_iw, 2*g_iw))
        g_bl_bcast = mpi.bcast(g_bl)
        assert_block_gfs_are_close(g_bl, g_bl_bcast)

if __name__ == '__main__':
    unittest.main()
