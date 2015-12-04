
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

from pytriqs.archive import *
from numpy import *
from pytriqs.utility.comparison_tests import *

d = {'a' : 1.0, 'b' : [1,2,3]}

with HDFArchive('hdf5_io.out.h5','w', init = d.items()) as h:
    h['c'] = 100
    h['d'] = array([[1,2,3],[4,5,6]])
    h['e'] = (1,2,3)
    h['f'] = { 'a':10, 'b': 20}
    h.create_group('g')
    g = h['g']
    g['a'] = 98
    g['b'] = (1,2,3)
    g['c'] = 200

h2 = HDFArchive('hdf5_io.out.h5','r')
dd = h2['f']
gg = h2['g']
del h2, gg

h3 = HDFArchive('hdf5_io.out.h5','a')
dd = h3['f']
dd['c'] = 18
h3['f'] = dd
gg = h3['g']
gg['d'] = 700
del h3, gg

gg = HDFArchive('hdf5_io.out.h5','a')['g']
gg['x'] = 1.5

with HDFArchive('hdf5_io.out.h5','a')['g'] as g:
    g['y'] = 'zzz'

# Final check
A = HDFArchive('hdf5_io.out.h5','r')
assert A['a'] == 1.0
assert A['b'] == [1,2,3]
assert A['c'] == 100
assert_arrays_are_close(A['d'], array([[1, 2, 3], [4, 5, 6]]))
assert A['e'] == (1,2,3)
assert A['f'] == {'a': 10, 'c': 18, 'b': 20}
assert A['g']['a'] == 98
assert A['g']['b'] == (1, 2, 3)
assert A['g']['c'] == 200
assert A['g']['d'] == 700
assert A['g']['x'] == 1.5
assert A['g']['y'] == 'zzz'
