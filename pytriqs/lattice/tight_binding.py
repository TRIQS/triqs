from __future__ import absolute_import

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

__all__ = ['BravaisLattice', 'BrillouinZone', 'TightBinding', 'dos', 'dos_patch', 'energies_on_bz_grid', 'energies_on_bz_path', 'energy_matrix_on_bz_path',
           'hopping_stack', 'TBLattice']

from .lattice_tools import BravaisLattice
from .lattice_tools import BrillouinZone
from .lattice_tools import TightBinding
from .lattice_tools import dos_patch as dos_patch_c
from .lattice_tools import dos as dos_c
from .lattice_tools import energies_on_bz_grid, energies_on_bz_path, hopping_stack, energy_matrix_on_bz_path
from pytriqs.dos import DOS
import numpy


def dos(tight_binding, n_kpts, n_eps, name) : 
    """
    :param tight_binding: a tight_binding object
    :param n_kpts: the number of k points to use in each dimension
    :param n_eps: number of points used in the binning of the energy
    :param name: name of the resulting dos

    :rtype: return a list of DOS, one for each band
    """
    eps, arr = dos_c(tight_binding, n_kpts, n_eps)
    return [ DOS (eps, arr[:, i], name) for i in range (arr.shape[1]) ]

def dos_patch(tight_binding, triangles, n_eps, n_div, name) :  
    """
    To be written
    """
    eps, arr = dos_patch_c(tight_binding, triangles, n_eps, n_div)
    return DOS (eps, arr, name)

# for backward compatibility. Not documented. 
class TBLattice:

    def __init__ (self, units, hopping, orbital_positions = [ (0, 0, 0) ], orbital_names = [""]):

        # the k are int32 which boost python does like to convert 
        def reg(k) : return tuple( int(x) for x in k) 
        self._hop = dict ( ( reg(k), numpy.array(v)) for k, v in hopping.items())
        orb = dict ( (str(i), orb) for (i, orb) in enumerate(orbital_positions ))
        self.bl = BravaisLattice(units, orbital_positions)
        self.bz = BrillouinZone(self.bl)
        self.tb = TightBinding(self.bl, self._hop) #, orbital_positions )
        self.dim = self.bl.dim
        self.NOrbitalsInUnitCell = self.bl.n_orbitals
        self.Units = units
        self.OrbitalPositions = orbital_positions 
        self.OrbitalNames = orbital_names
        self.MuPattern = numpy.identity(self.NOrbitalsInUnitCell)

    def latt_to_real_x(self, p) : 
        return self.bl.lattice_to_real_coordinates (numpy.array(p, numpy.float64))
        # modified since array are not converted automatically any more
        ##return self.bl.lattice_to_real_coordinates (p ) #numpy.array(p.float64))

    def hopping_dict(self) : return self._hop

    def hopping(self, k_stack) :
        return hopping_stack(self.tb, k_stack)

    #def dos(self) : d = dos (TB, nkpts= 100, neps = 100, name = 'dos2')


