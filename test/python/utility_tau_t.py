# Copyright (c) 2026 Simons Foundation
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

import unittest
from triqs.utility.utilities import TauT
from h5 import HDFArchive

beta = 10.0
# Largest grid index (tau_t::n_max == 2**64 - 1), corresponding to tau = beta.
NMAX = 2**64 - 1

# beta is a static member shared by all tau_t instances; set it once for the suite.
TauT.set_beta(beta)

class test_tau_t(unittest.TestCase):

    def test_default(self):
        # Default construction is tau = 0 (grid index 0).
        self.assertEqual(TauT().get_n(), 0)
        self.assertEqual(str(TauT()), "0 [tau_t : beta = 10 n = 0]")

    def test_static_beta(self):
        # beta is shared across all instances via the static accessor.
        self.assertEqual(TauT.get_beta_value(), beta)

    def test_factories(self):
        # zero / epsilon / beta map to the boundary grid indices.
        self.assertEqual(TauT.zero().get_n(), 0)
        self.assertEqual(TauT.epsilon().get_n(), 1)
        self.assertEqual(TauT.beta().get_n(), NMAX)

    def test_repr_fields(self):
        # str/repr exposes the (shared) beta and the integer grid index n.
        self.assertEqual(str(TauT(0)), "0 [tau_t : beta = 10 n = 0]")
        self.assertIn("n = %d" % NMAX, str(TauT(NMAX)))

    def test_equality_is_on_grid_index(self):
        # Comparison uses the exact integer index, not the (lossy) double value.
        self.assertEqual(TauT(0), TauT(0))
        self.assertEqual(TauT(5), TauT(5))
        self.assertNotEqual(TauT(0), TauT(NMAX))

    def test_ordering(self):
        lo   = TauT(0)
        half = TauT(NMAX // 2)
        hi   = TauT(NMAX)
        self.assertTrue(lo < half < hi)
        self.assertTrue(hi > half > lo)
        self.assertTrue(lo <= TauT(0))
        self.assertTrue(hi >= half)

    def test_from_double(self):
        # from_double maps [0, beta] onto the integer grid; endpoints are exact.
        self.assertEqual(TauT.from_double(0.0).get_n(), 0)
        self.assertEqual(TauT.from_double(beta).get_n(), NMAX)

    def test_h5_roundtrip(self):
        tp = TauT(NMAX // 3)
        with HDFArchive("tau_t.h5", 'w') as ar:
            ar['tp'] = tp
        with HDFArchive("tau_t.h5", 'r') as ar:
            tp2 = ar['tp']
        self.assertIsInstance(tp2, TauT)
        self.assertEqual(tp, tp2)

if __name__ == '__main__':
    unittest.main()
