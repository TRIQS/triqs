# Copyright (c) 2021-2023 Simons Foundation
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
# Authors: Collins Kariuki, Nils Wentzell

import unittest
import sys
from triqs.lattice import BravaisLattice, BrillouinZone, TightBinding
from triqs.lattice.tight_binding import TBLattice
from triqs.lattice.utils import TB_to_sympy
from h5 import HDFArchive

import numpy as np

class test_lattice_tight_binding(unittest.TestCase):

    def setUp(self):
        self.units = [(2, 1), (1, 2)]

        norb = 2
        self.hoppings = {(0, 1): np.eye(norb),
                         (0, -1): np.eye(norb),
                         (1, 0):  np.ones((norb, norb)),
                         (-1, 0): np.ones((norb, norb))
                         }
        self.orbital_positions = [(0, 0)]*norb
        self.orbital_names = [""]*norb

    def test_TBLattice(self):

        tbl = TBLattice(units=self.units, hoppings=self.hoppings,
                        orbital_positions=self.orbital_positions,
                        orbital_names=self.orbital_names
                        )
        print(tbl)

        self.assertEqual(list(self.hoppings.keys()), list(tbl.hoppings.keys()))
        self.assertTrue(all(map(np.array_equal, self.hoppings.values(), tbl.hoppings.values())))

        # Make sure manual BravaisLattice / TightBinding / BrillouinZone
        # construction yields same objects 

        bl = BravaisLattice(
            self.units, self.orbital_positions, self.orbital_names)
        bz = BrillouinZone(bl)
        tb = TightBinding(bl, self.hoppings)

        self.assertEqual(bl, tbl.bl)
        self.assertEqual(bz, tbl.bz)
        self.assertEqual(tb, tbl.tb)

        # Test H5 Read / Write

        with HDFArchive("tbl.h5", 'w') as arch:
            arch['tbl'] = tbl

        with HDFArchive("tbl.h5", 'r') as arch:
            tbl_read = arch['tbl']

        self.assertEqual(tbl, tbl_read)
    
    def test_TB_to_sympy_2D(self):
        try:
            import sympy as sp
            from sympy import cos
        except ImportError:
            print("ImportError: SymPy needs to be installed to run this test")
            return

        kx, ky = sp.symbols("kx ky", real = True)

        tbl = TBLattice(units=self.units, hoppings=self.hoppings,
                        orbital_positions=self.orbital_positions,
                        orbital_names=self.orbital_names
                        )
        
        # testing numerical output
        tbl_numerical = (TB_to_sympy(tbl, analytical = False, precision = 3)).tolist()
        self.assertEquals(tbl_numerical,
                          [[2.0*cos(1.0*kx + 2.0*ky) + 2.0*cos(2.0*kx + 1.0*ky), 2.0*cos(2.0*kx + 1.0*ky)], 
                           [2.0*cos(2.0*kx + 1.0*ky), 2.0*cos(1.0*kx + 2.0*ky) + 2.0*cos(2.0*kx + 1.0*ky)]]
                          )
        


if __name__ == '__main__':
    unittest.main()
