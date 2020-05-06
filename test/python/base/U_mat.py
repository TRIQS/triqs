################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011 by M. Aichhorn, L. Pourovskii, V. Vildosola
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

from h5 import *
from triqs.operators.util import *
from triqs.utility.comparison_tests import *
import numpy

U_sph = U_matrix(l=2, U_int=2.0, J_hund=0.5)
U_cubic = transform_U_matrix(U_sph,spherical_to_cubic(l=2))
U,Up = reduce_4index_to_2index(U_cubic)

with HDFArchive('U_mat.ref.h5', 'r') as ar:
   assert_arrays_are_close(ar['Ufull_sph'], U_sph)
   assert_arrays_are_close(ar['Ufull_cubic'], U_cubic)
   assert_arrays_are_close(ar['U'], U)
   assert_arrays_are_close(ar['Up'], Up)
