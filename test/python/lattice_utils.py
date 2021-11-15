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
# Authors: Nils Wentzell

import unittest
from triqs.lattice import BravaisLattice, BrillouinZone
from triqs.lattice.utils import k_space_path

import numpy as np

class test_utils(unittest.TestCase):

    def test_k_space_path(self):

        units = np.array([[1.0, 1.0, 0.0], [0.5, 0.0, 0.5], [0., 1.5, 1.5]])
        bl    = BravaisLattice(units=units)
        bz    = BrillouinZone(bl)

        Gamma = np.array([0.0, 0.0, 0.0])
        M     = np.array([0.5, 0.5, 0.0])
        R     = np.array([0.5, 0.5, 0.5])
        X     = np.array([0.5, 0.0, 0.0])
        Z     = np.array([0.0, 0.0, 0.5])

        # ---- 

        num = 101
        paths = [(Gamma, M), (M, R), (X, Z)]
        kvecs, dist = k_space_path(paths, num=num, bz=bz)

        dist_cmp_exact = [0.]
        for n, (ki, kf) in enumerate(paths):
            dk = kf - ki

            kvec = kvecs[n*num:(n+1)*num]
            kvec_exact = ki[None,:] + np.linspace(0,1,num)[:,None] * dk[None,:]
            self.assertTrue(np.array_equal(kvec, kvec_exact))
            
            dd = np.linalg.norm(np.dot(dk, bz.units))
            dist_cmp_exact.append(dd + dist_cmp_exact[-1]) 

        dist_cmp = np.concatenate((dist[0:1], dist[num-1::num]))
        self.assertTrue(np.array_equal(dist_cmp, dist_cmp_exact))

if __name__ == '__main__':
    unittest.main()