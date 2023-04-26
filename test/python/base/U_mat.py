# Copyright (c) 2015 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2015 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020 Simons Foundation
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
# Authors: Michel Ferrero, Priyanka Seth, Nils Wentzell


from h5 import *
from triqs.operators.util import *
from triqs.utility.comparison_tests import *
import numpy

U_sph = U_matrix_slater(l=2, U_int=2.0, J_hund=0.5)
U_cubic = transform_U_matrix(U_sph,spherical_to_cubic(l=2))
U,Up = reduce_4index_to_2index(U_cubic)

with HDFArchive('U_mat.ref.h5', 'r') as ar:
   assert_arrays_are_close(ar['Ufull_sph'], U_sph)
   assert_arrays_are_close(ar['Ufull_cubic'], U_cubic)
   assert_arrays_are_close(ar['U'], U)
   assert_arrays_are_close(ar['Up'], Up)
