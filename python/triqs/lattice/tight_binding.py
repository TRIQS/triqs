# Copyright (c) 2013-2014 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2014 Centre national de la recherche scientifique (CNRS)
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
# Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell


__all__ = ['BravaisLattice', 'BrillouinZone', 'TightBinding', 'dos', 'dos_patch', 'energies_on_bz_grid', 'energies_on_bz_path', 'energy_matrix_on_bz_path',
           'hopping_stack', 'TBLattice']

from ..gf import Gf, MeshBrZone
from .lattice_tools import BravaisLattice
from .lattice_tools import BrillouinZone
from .lattice_tools import TightBinding
from .lattice_tools import dos_patch as dos_patch_c
from .lattice_tools import dos as dos_c
from .lattice_tools import energies_on_bz_grid, energies_on_bz_path, hopping_stack, energy_matrix_on_bz_path
from triqs.dos import DOS
import numpy
import warnings


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

class TBLattice(object):

    """ Class describing a tight binding hamiltonian on top of a bravais lattice.

    Parameters
    ----------

    units : list of tuples of floats
        Basis vectors for the real space lattice.

    hopping : dict
        Dictionary with tuples of integers as keys,
        describing real space hoppings in multiples of
        the real space lattice basis vectors, and values being
        numpy ndarray hopping matrices in the orbital indices.

    orbital_positions : list of three three-tuples of floats.
        Internal orbital positions in the unit-cell.

    orbital_names : list of strings
        Names for each orbital.
    """

    def __init__ (self, units, hopping, orbital_positions = [(0, 0, 0)], orbital_names = [""]):

        # the k are int32 which boost python does like to convert
        def reg(k) : return tuple( int(x) for x in k)
        self._hop = dict ( ( reg(k), numpy.array(v)) for k, v in list(hopping.items()))
        orb = dict ( (str(i), orb) for (i, orb) in enumerate(orbital_positions ))
        self.bl = BravaisLattice(units, orbital_positions)
        self.bz = BrillouinZone(self.bl)
        self.tb = TightBinding(self.bl, self._hop) #, orbital_positions )
        self.ndim = self.bl.ndim
        self.NOrbitalsInUnitCell = self.bl.n_orbitals
        self.Units = units
        self.OrbitalPositions = orbital_positions
        self.OrbitalNames = orbital_names
        self.MuPattern = numpy.identity(self.NOrbitalsInUnitCell)

    def latt_to_real_x(self, p):
        return self.bl.lattice_to_real_coordinates(numpy.array(p, numpy.float64))

    def hopping_dict(self): return self._hop


    def fourier(self, arg):
        """ C.f. documentation of TightBinding.fourier
        """
        return self.tb.fourier(arg)

    def dispersion(self, arg):
        """ C.f. documentation of TightBinding.dispersion
        """
        return self.tb.dispersion(arg)

    def hopping(self, k_stack) :
        warnings.warn("TBLattice.hopping(k_stack) is deprecated; use TBLattice.dispersion(k) instead.", warnings.DeprecationWarning)
        return hopping_stack(self.tb, k_stack)

