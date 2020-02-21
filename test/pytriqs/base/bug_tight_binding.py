
""" Test the hopping matrix size control in the tight binding class. """
from __future__ import unicode_literals

import numpy as np
from pytriqs.lattice.tight_binding import TightBinding, BravaisLattice

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
