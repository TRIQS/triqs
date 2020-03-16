import copy
import numpy as np

from pytriqs.gf import MeshBrillouinZone
from pytriqs.lattice import BrillouinZone, BravaisLattice

cell = [
    [1., 0., 2.],
    [0.5, 2.5, 1.],
    [1., 2., 0.]]

bl = BravaisLattice(cell)
bz = BrillouinZone(bl)

periodization_matrix = 32 * np.eye(3, dtype=np.int32)
bzmesh = MeshBrillouinZone(bz, periodization_matrix)

bzmesh_ref = copy.deepcopy(bzmesh) # BREAKS

units = bzmesh.domain.units()
units_ref = bzmesh_ref.domain.units()

np.testing.assert_array_almost_equal(units, units_ref)

