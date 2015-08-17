
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


d = {'a' : 1.0, 'b' : [1,2,3]}

with HDFArchive('hdf5_io.output.h5','w', init = d.items()) as h:
    h['c'] = 100
    h['d'] = array([[1,2,3],[4,5,6]])
    h['e'] = (1,2,3)
    h['f'] = { 'a':10, 'b': 20}
    h.create_group('g')
    g = h['g']
    g['a'] = 98
    g['b'] = (1,2,3)
    g['c'] = 200

h2 = HDFArchive('hdf5_io.output.h5','r')
dd = h2['f']
gg = h2['g']
del h2, gg

h3 = HDFArchive('hdf5_io.output.h5','a')
dd = h3['f']
dd['c'] = 18
h3['f'] = dd
gg = h3['g']
gg['d'] = 700
del h3, gg

gg = HDFArchive('hdf5_io.output.h5','a')['g']
gg['x'] = 1.5
