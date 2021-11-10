# Copyright (c) 2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2016 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2019-2020 Simons Foundation
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
# Authors: Nils Wentzell, tayral

from numpy import *
from numpy.linalg import *
from triqs.gf import *
from triqs.lattice import *
from math import pi

# ---- square lattice : 16 x 16 x 1
units = matrix([[1.,0.,0.],[0.,1.,0.],[0.,0.,1.]], float)
per_mat = matrix([[16, 0, 0],[0, 16, 0],[0, 0, 1]], int32)
bz=BrillouinZone(BravaisLattice(units))
k_mesh = MeshBrZone(bz, per_mat)

assert all(k_mesh.closest_index([pi/2,  pi, 0.0]) == [4, 8, 0])
assert all(k_mesh.closest_index([pi/8,2*pi, 0.0]) == [1, 16, 0])

# ---- triangular lattice : 6 x 6 x 1
units = matrix([[1.,0.,0.],[0.5,sqrt(3)/2.,0.],[0.,0.,1.]], float)
per_mat=  matrix([[6, 6, 0],[-6, 6, 0],[0, 0, 1]], int32)
bz=BrillouinZone(BravaisLattice(units))
k_mesh = MeshBrZone(bz, per_mat)

assert all(k_mesh.closest_index([0.,5.,0.]) == [4, 4, 0])
assert all(k_mesh.closest_index([2.,1.,0.]) == [4, 0, 0])
assert all(k_mesh.closest_index([2.,0.,0.]) == [3, -1, 0])
