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


from h5.formats import register_class
__all__ = ['BravaisLattice', 'BrillouinZone', 'TightBinding', 'dos', 'dos_patch', 'energies_on_bz_grid', 'energies_on_bz_path', 'energy_matrix_on_bz_path',
           'hopping_stack', 'TBLattice']

from ..gf import Gf, MeshBrZone, MeshCycLat
from .lattice_tools import BravaisLattice
from .lattice_tools import BrillouinZone
from .lattice_tools import TightBinding
from .lattice_tools import dos_patch as dos_patch_c
from .lattice_tools import dos as dos_c
from .lattice_tools import energies_on_bz_grid, energies_on_bz_path, hopping_stack, energy_matrix_on_bz_path
from triqs.dos import DOS
import numpy
import warnings


def dos(tight_binding, n_kpts, n_eps, name):
    """
    :param tight_binding: a tight_binding object
    :param n_kpts: the number of k points to use in each dimension
    :param n_eps: number of points used in the binning of the energy
    :param name: name of the resulting dos

    :rtype: return a list of DOS, one for each band
    """
    eps, arr = dos_c(tight_binding, n_kpts, n_eps)
    return [DOS(eps, arr[:, i], name) for i in range(arr.shape[1])]


def dos_patch(tight_binding, triangles, n_eps, n_div, name):
    """
    To be written
    """
    eps, arr = dos_patch_c(tight_binding, triangles, n_eps, n_div)
    return DOS(eps, arr, name)


class TBLattice(object):

    """ Class describing a tight binding hamiltonian on top of a bravais lattice.
    Has objects of type BravaisLattice, BrillouinZone and TightBinding as attributes,
    and exposes part of their interfaces.

    Attributes
    ----------

    bl : BravaisLattice
        The associated Bravais Lattice

    bz : BrillouinZone
        The associated Brillouin Zone

    tb : TightBinding
        The tight-binding Hamiltonian
    """

    def __init__(self, units,
                 hoppings=dict(),
                 orbital_positions=[(0, 0, 0)],
                 orbital_names=None,
                 hopping=None):
        """
        Parameters
        ----------

        units : list of tuples of floats
            Basis vectors for the real space lattice.

        hoppings : dict
            Dictionary with tuples of integers as keys,
            describing real space hoppings in multiples of
            the real space lattice basis vectors, and values being
            numpy ndarray hopping matrices in the orbital indices.

        orbital_positions : list of three three-tuples of floats.
            Internal orbital positions in the unit-cell.

        orbital_names : list of strings
            Names for each orbital.
        """

        if hopping is not None:
            warnings.warn(
                "Keyword hopping in TBLattice.__init__ deprecated; use hoppings instead.", DeprecationWarning)
            hoppings = hopping

        if orbital_names is None:
            orbital_names = len(orbital_positions) * [""]

        self.bl = BravaisLattice(units, orbital_positions, orbital_names)
        self.bz = BrillouinZone(self.bl)
        self.tb = TightBinding(self.bl, hoppings)

    @property
    def hoppings(self):
        return {tuple(displ): hop for displ, hop in zip(self.tb.displ_vec, self.tb.overlap_mat_vec)}

    def get_kmesh(self, n_k):
        """Return a mesh on the Brillouin zone with a given discretization

        Parameters
        ----------
        n_k : int or three-tuple of int
            The linear dimension(s)

        Returns
        -------
        MeshBrZone
            The mesh on the Brillouin zone

        """
        return MeshBrZone(self.bz, n_k)

    def get_rmesh(self, n_r):
        """Return a mesh on the Bravais lattice with a given periodicity

        Parameters
        ----------
        n_r : int or three-tuple of int
            The periodicity in each dimension

        Returns
        -------
        MeshCycLat
            The cyclic lattice mesh

        """
        return MeshCycLat(self.bl, n_r)

    # ---- Expose BravaisLattice API ----

    @property
    def ndim(self):
        """Number of dimensions of the lattice"""
        return self.bl.ndim

    @property
    def units(self):
        """Number of dimensions of the lattice"""
        return self.bl.units[:self.ndim, :self.ndim]

    @property
    def n_orbitals(self):
        """Number of orbitals in the unit cell"""
        return self.bl.n_orbitals

    @property
    def orbital_positions(self):
        """The list of orbital positions"""
        return self.bl.orbital_positions

    @property
    def orbital_names(self):
        """The list of orbital names"""
        return self.bl.orbital_names

    # ---- Expose TightBinding API ----

    def lattice_to_real_coordinates(self, x):
        return self.tb.lattice_to_real_coordinates(x)
    lattice_to_real_coordinates.__doc__ = TightBinding.lattice_to_real_coordinates.__doc__

    def fourier(self, arg):
        return self.tb.fourier(arg)
    fourier.__doc__ = TightBinding.fourier.__doc__

    def dispersion(self, arg):
        return self.tb.dispersion(arg)
    dispersion.__doc__ = TightBinding.dispersion.__doc__

    # ---- H5 Serialization ----

    def __reduce_to_dict__(self):
        return {"BravaisLattice": self.bl,
                "BrillouinZone": self.bz,
                "TightBinding": self.tb}

    @classmethod
    def __factory_from_dict__(cls, name, d):
        bl = d["BravaisLattice"]
        tb = d["TightBinding"]
        hoppings = {tuple(displ): hop for displ, hop in zip(
            tb.displ_vec, tb.overlap_mat_vec)}
        return cls(bl.units[:bl.ndim, :bl.ndim],
                   hoppings,
                   bl.orbital_positions,
                   bl.orbital_names)

    # ---- Comparison ----

    def __eq__(self, other):
        if not isinstance(other, TBLattice):
            return False
        return self.tb == other.tb and self.bz == other.bz and self.bl == other.bl

    # ---- Print ----

    def __str__(self):
        return str(self.tb)

    # ---- Backward Compatibility ----

    @property
    def Units(self):
        warnings.warn(
            "TBLattice.Units is deprecated; use TBLattice.units instead.", DeprecationWarning)
        return self.units

    @property
    def NOrbitalsInUnitCell(self):
        warnings.warn(
            "TBLattice.tb is deprecated; use TBLattice.n_orbitals instead.", DeprecationWarning)
        return self.n_orbitals

    @property
    def OrbitalPositions(self):
        warnings.warn(
            "TBLattice.OrbitalPositions is deprecated; use TBLattice.orbital_positions instead.", DeprecationWarning)
        return self.orbital_positions

    @property
    def OrbitalNames(self):
        warnings.warn(
            "TBLattice.OrbitalNames is deprecated; use TBLattice.orbital_names instead.", DeprecationWarning)
        return self.orbital_names

    def hopping_dict(self):
        warnings.warn(
            "TBLattice.hopping_dict() is deprecated; use TBLattice.hoppings instead.", DeprecationWarning)
        return self.hoppings

    def hopping(self, k):
        warnings.warn(
            "TBLattice.hopping(k) is deprecated; use TBLattice.dispersion(k) instead.", DeprecationWarning)
        return hopping_stack(self, k)


register_class(TBLattice)
