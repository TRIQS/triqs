# Copyright (c) 2020-2022 Simons Foundation
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
# Authors: Alexander Hampel, Nils Wentzell

from triqs.gf import *
from triqs.utility.comparison_tests import *
import numpy as np

iw_mesh = MeshImFreq(beta=40, S='Fermion', n_iw=1025)

# create upfolded Gf with 5x5 dim
G_upf = Gf(mesh=iw_mesh, target_shape=[5, 5])
# create downfolded target Gf with 3x3 dim
G_dnf = Gf(mesh=iw_mesh, target_shape=[3, 3])

# downfolding projector from the 5 to 3 orb
downfolding = Gf(mesh=iw_mesh, target_shape=[3,5])

# the downfolding / projection from 5 to 3 orbitals fails
# because atm triqs expects in each of those multiplications that
# dim a = dim b
# but this should work since we are doing [3,5]x[5,5]x[5,3]
G_dnf  << downfolding*(G_upf*downfolding.transpose().conjugate())

