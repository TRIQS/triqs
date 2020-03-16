
################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011 by M. Ferrero, O. Parcollet
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
from numpy import *

from pytriqs.archive import *
from pytriqs.utility.comparison_tests import *

class TestHdf5Io(unittest.TestCase):

    def test_hdf5_io(self):
        d = {'dbl' : 1.0, 'lst' : [1,[1],'a']}
        
        # === Write to archive
        with HDFArchive('hdf5_io.out.h5','w', init = list(d.items())) as arch:
        
            arch['int'] = 100
            arch['arr'] = array([[1,2,3],[4,5,6]])
            arch['tpl'] = (2,[2],'b')
            arch['dct'] = { 'a':[10], 'b':20 }
        
            arch.create_group('grp')
            grp = arch['grp']
            grp['int'] = 98
            grp['tpl'] = (3,[3],'c')
            grp['dct'] = { 'a':[30], 'b':40 }
        
        # === Read access
        with HDFArchive('hdf5_io.out.h5','r') as arch:  
            dct = arch['dct']
            grp = arch['grp']
        
        # === Read/Write access
        with HDFArchive('hdf5_io.out.h5','a') as arch:
        
            dct = arch['dct']
            dct['c'] = 'triqs'
            arch['dct'] = dct
        
            grp = arch['grp']
            dct = grp['dct']
            dct['c'] = 'qmc'
        
            grp['dct'] = dct
            grp['d'] = 700
            grp['x'] = 1.5
            grp['y'] = 'zzz'
        
        
        # === Final check
        arch = HDFArchive('hdf5_io.out.h5','r')
        self.assertEqual( arch['dbl'] , 1.0 )
        self.assertEqual( arch['lst'] , [1,[1],'a'] )
        self.assertEqual( arch['int'] , 100 )
        assert_arrays_are_close( arch['arr'] , array([[1, 2, 3], [4, 5, 6]]) )
        self.assertEqual( arch['tpl'] , (2,[2],'b') )
        self.assertEqual( arch['dct'] , {'a':[10], 'b':20, 'c':'triqs'} )
        self.assertEqual( arch['grp']['int'] , 98 )
        self.assertEqual( arch['grp']['tpl'] , (3,[3],'c') )
        self.assertEqual( arch['grp']['dct'] , { 'a':[30], 'b':40, 'c':'qmc'} )
        self.assertEqual( arch['grp']['d'] , 700 )
        self.assertEqual( arch['grp']['x'] , 1.5 )
        self.assertEqual( arch['grp']['y'] , 'zzz' )

if __name__ == '__main__':
    unittest.main()
