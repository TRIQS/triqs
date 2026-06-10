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
from triqs.utility.utilities import TimePt
from h5 import HDFArchive

beta = 10.0
# Largest grid index (time_pt::Nmax == 2**64 - 1), corresponding to tau = beta.
NMAX = 2**64 - 1

class test_time_pt(unittest.TestCase):

    def test_default(self):
        # Default construction is tau = 0 on a zero segment.
        self.assertEqual(str(TimePt()), "0 [time_pt : beta = 0 n = 0]")

    def test_repr_fields(self):
        # str/repr exposes the stored beta and integer grid index n.
        self.assertEqual(str(TimePt(0, beta)), "0 [time_pt : beta = 10 n = 0]")
        self.assertIn("n = %d" % NMAX, str(TimePt(NMAX, beta)))

    def test_equality_is_on_grid_index(self):
        # Comparison uses the exact integer index, not the (lossy) double value,
        # and is independent of beta.
        self.assertEqual(TimePt(0, beta), TimePt(0, beta))
        self.assertEqual(TimePt(5, beta), TimePt(5, 2 * beta))
        self.assertNotEqual(TimePt(0, beta), TimePt(NMAX, beta))

    def test_ordering(self):
        lo   = TimePt(0, beta)
        half = TimePt(NMAX // 2, beta)
        hi   = TimePt(NMAX, beta)
        self.assertTrue(lo < half < hi)
        self.assertTrue(hi > half > lo)
        self.assertTrue(lo <= TimePt(0, beta))
        self.assertTrue(hi >= half)

    def test_h5_roundtrip(self):
        tp = TimePt(NMAX // 3, beta)
        with HDFArchive("time_pt.h5", 'w') as ar:
            ar['tp'] = tp
        with HDFArchive("time_pt.h5", 'r') as ar:
            tp2 = ar['tp']
        self.assertIsInstance(tp2, TimePt)
        self.assertEqual(tp, tp2)

if __name__ == '__main__':
    unittest.main()
