################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2017 by H. U.R. Strand
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

import numpy as np
from pytriqs.archive import HDFArchive

filename = 'h5archive.h5'

p = dict(
    my_flag=True,
    my_int=1,
    my_long=1L,
    my_float=1.,
    my_complex=1.j,
    my_string='foobar',
    my_string_unicode=u'foobar',
    my_ndarray_int=np.array([1]),
    my_ndarray_float=np.array([1.]),
    my_ndarray_complex=np.array([1.j]),
    )

with HDFArchive(filename, 'w') as a:
    a['p'] = p

with HDFArchive(filename, 'r') as a:
    p_ref = a['p']

for key in p.keys():
    print key, type(p[key]), type(p_ref[key])
    assert( type(p[key]) == type(p_ref[key]) )

    if type(p[key]) == np.ndarray:
        assert( p[key].dtype == p_ref[key].dtype )
        print 'dtypes: ', p[key].dtype, p_ref[key].dtype

        
    
