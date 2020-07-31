# Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2020 Simons Foundation
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
# Authors: Hugo U. R. Strand, Nils Wentzell


""" Test the hopping matrix size control in the tight binding class. """

import numpy as np
from triqs.lattice.tight_binding import TightBinding, BravaisLattice

number_of_orbitals = 4

bl = BravaisLattice(
    units = [(1,0,0),(0,1,0)],
    orbital_positions = [(0,0,0)]*number_of_orbitals)

# Hopping matrix with wrong size!!!!
hopping = { (0, 0): np.eye(2) }

got_dimension_error = False

try:
    tb = TightBinding(bl, hopping)
except:
    got_dimension_error = True

assert( got_dimension_error )
