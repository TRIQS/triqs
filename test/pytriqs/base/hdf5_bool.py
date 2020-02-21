from __future__ import unicode_literals
################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2017 by N. Wentzell, O. Parcollet
#
# TRIQS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# TRIQS. If not, see <http://www.gnu.org/licenses/>.
#
################################################################################

import unittest
import numpy as np

from pytriqs.archive import *

class TestHdf5Bool(unittest.TestCase):

    def test_write_read_bool(self):

        # Write
        with HDFArchive('bool.h5','w') as arch: 
            arch['t'] = True
            arch['f'] = False 
            arch['i'] = 10

        with HDFArchive('bool.h5','r') as arch: 
            t = arch['t']
            f = arch['f'] 
            i = arch['i']

        self.assertTrue(t) 
        self.assertFalse(f)
        self.assertIs(type(t),bool)
        self.assertIs(type(f),bool)
        self.assertIs(type(i),int)

if __name__ == '__main__':
    unittest.main()
