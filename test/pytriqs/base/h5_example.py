
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

print "bonjour"

d = {'a' : 1.0, 'b' : [1,2,3]}

h = HDFArchive('h5_example.output.h5','w', init = d.items())

h['c'] = 100
h['d'] = array([[1,2,3],[4,5,6]])
h['e'] = (1,2,3)
h['f'] = { 'a':10, 'b': 20}
h.create_group('g')
g = h['g']
g['a'] = 98
g['b'] = (1,2,3)
g['c'] = 200

del h
