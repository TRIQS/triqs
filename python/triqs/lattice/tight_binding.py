# Copyright (c) 2013 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2019-2023 Simons Foundation
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
# Authors: Michel Ferrero, Alexander Hampel, Olivier Parcollet, Hugo U.R. Strand, Nils Wentzell

"""Tight-binding Hamiltonians on Bravais lattices.

Provides the higher-level Python wrapper :class:`TBLattice` that bundles a 
:class:`BravaisLattice`, :class:`BrillouinZone` and :class:`TightBinding` from 
:mod:`triqs.lattice.lattice_tools` together.

Also exposes the density-of-states helpers :func:`dos` (over a regular k-grid)
and :func:`dos_patch` (over a triangular Brillouin-zone patch), which wrap the
underlying C++ routines and return :class:`triqs.dos.DOS` objects.
"""

from h5.formats import register_class
__all__ = ['BravaisLattice', 'BrillouinZone', 'TightBinding', 'dos', 'TBLattice']

from ..gfs import Gf, MeshBrZone, MeshCycLat
from .lattice_tools import BravaisLattice
from .lattice_tools import BrillouinZone
from .lattice_tools import TightBinding
from .lattice_tools import dos_patch as dos_patch_c
from .lattice_tools import dos as dos_c
from triqs.dos import DOS
import numpy
import warnings


def dos(tight_binding, n_kpts, n_eps, name):
    """Compute the density of states of a tight-binding Hamiltonian on a regular k-grid.

    Wraps the C++ :func:`triqs.lattice.lattice_tools.dos` and splits the result
    into one :class:`triqs.dos.DOS` per band.

    Parameters
    ----------
    tight_binding : TightBinding
        The tight-binding Hamiltonian.
    n_kpts : int
        Number of k-points along each dimension of the Brillouin zone.
    n_eps : int
        Number of energy bins.
    name : str
        Name of the resulting DOS objects.

    Returns
    -------
    list of triqs.dos.DOS
        One density of states per band.
    """
    eps, arr = dos_c(tight_binding, n_kpts, n_eps)
    return [DOS(eps, arr[:, i], name) for i in range(arr.shape[1])]


def dos_patch(tight_binding, triangles, n_eps, n_div, name):
    """Compute the density of states of a tight-binding Hamiltonian on a triangular Brillouin-zone patch.

    Only supported for 2-dimensional lattices.

    Parameters
    ----------
    tight_binding : TightBinding
        The tight-binding Hamiltonian.
    triangles : numpy.ndarray
        2-D array of shape ``(n_triangles * 3, 2)`` containing the vertices of
        the triangular patches in the Brillouin zone, three rows per triangle.
    n_eps : int
        Number of energy bins.
    n_div : int
        Number of sub-divisions of each triangle used for the sampling.
    name : str
        Name of the resulting DOS object.

    Returns
    -------
    triqs.dos.DOS
        The density of states summed over orbitals on the given patch.
    """
    eps, arr = dos_patch_c(tight_binding, triangles, n_eps, n_div)
    return DOS(eps, arr, name)


class TBLattice(object):
    """A tight-binding Hamiltonian on top of a Bravais lattice.

    Bundles a :class:`BravaisLattice`, a :class:`BrillouinZone` and a
    :class:`TightBinding` instance and exposes part of their interfaces directly.

    Parameters
    ----------
    units : list of tuples of floats
        Basis vectors of the real-space lattice.
    hoppings : dict, optional
        Dictionary mapping tuples of integers (real-space displacements in
        multiples of the lattice basis vectors) to numpy ndarray hopping
        matrices over the orbital indices.
    orbital_positions : list of three-tuples of floats, optional
        Internal orbital positions in the unit cell.
    orbital_names : list of str, optional
        Names for each orbital.

    Attributes
    ----------
    bl : BravaisLattice
        The associated Bravais lattice.
    bz : BrillouinZone
        The associated Brillouin zone.
    tb : TightBinding
        The tight-binding Hamiltonian.
    hoppings : dict
        Real-space hoppings as a ``{displacement: matrix}`` dict.
    ndim : int
        Number of spatial dimensions of the lattice.
    units : numpy.ndarray
        ``(ndim, ndim)`` array whose rows are the lattice basis vectors.
    n_orbitals : int
        Number of orbitals in the unit cell.
    orbital_positions : list
        Positions of the orbitals inside the unit cell.
    orbital_names : list of str
        Names of the orbitals in the unit cell.
    """
    def __init__(self, units,
                 hoppings=dict(),
                 orbital_positions=[(0, 0, 0)],
                 orbital_names=None,
                 hopping=None):

        if hopping is not None:
            warnings.warn(
                "Keyword hopping in TBLattice.__init__ deprecated; use hoppings instead.", FutureWarning)
            hoppings = hopping

        if orbital_names is None:
            orbital_names = len(orbital_positions) * [""]

        self.bl = BravaisLattice(units, orbital_positions, orbital_names)
        self.bz = BrillouinZone(self.bl)
        self.tb = TightBinding(self.bl, hoppings)

    @property
    def hoppings(self):
        """Real-space hoppings as a ``{displacement: matrix}`` dict.

        Each key is a tuple of integers giving a real-space displacement in
        multiples of the lattice basis vectors; the corresponding value is the
        hopping matrix between orbitals at that displacement.

        Returns
        -------
        dict
            Mapping from displacement tuples to ``numpy.ndarray`` hopping
            matrices of shape ``(n_orbitals, n_orbitals)``.
        """
        return {tuple(displ): hop for displ, hop in zip(self.tb.displ_vec, self.tb.overlap_mat_vec)}

    def get_kmesh(self, n_k):
        """Return a mesh on the Brillouin zone with a given discretization.

        Parameters
        ----------
        n_k : int or three-tuple of int
            The linear dimension(s).

        Returns
        -------
        MeshBrZone
            The mesh on the Brillouin zone.

        """
        return MeshBrZone(self.bz, n_k)

    def get_rmesh(self, n_r):
        """Return a mesh on the Bravais lattice with a given periodicity.

        Parameters
        ----------
        n_r : int or three-tuple of int
            The periodicity in each dimension.

        Returns
        -------
        MeshCycLat
            The cyclic lattice mesh.

        """
        return MeshCycLat(self.bl, n_r)

    # ---- Expose BravaisLattice API ----

    @property
    def ndim(self):
        """Number of spatial dimensions of the lattice.

        Returns
        -------
        int
            Number of dimensions (1, 2 or 3).
        """
        return self.bl.ndim

    @property
    def units(self):
        """Lattice basis vectors as a ``(ndim, ndim)`` array in the standard basis.

        Returns
        -------
        numpy.ndarray
            Two-dimensional array whose rows are the basis vectors
            :math:`\\{ \\mathbf{a}_1, \\dots, \\mathbf{a}_d \\}` of the
            Bravais lattice.
        """
        return self.bl.units[:self.ndim, :self.ndim]

    @property
    def n_orbitals(self):
        """Number of orbitals in the unit cell.

        Returns
        -------
        int
            Number of atomic orbitals in the unit cell.
        """
        return self.bl.n_orbitals

    @property
    def orbital_positions(self):
        """Positions of the orbitals inside the unit cell.

        Returns
        -------
        list
            Orbital positions :math:`\\{ \\mathbf{r}_1, \\dots, \\mathbf{r}_m \\}`
            in the standard basis.
        """
        return list(self.bl.orbital_positions)

    @property
    def orbital_names(self):
        """Names of the orbitals in the unit cell.

        Returns
        -------
        list of str
            One name per orbital, in the same order as :attr:`orbital_positions`.
        """
        return list(self.bl.orbital_names)

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
                   list(bl.orbital_positions),
                   list(bl.orbital_names))

    # ---- Comparison ----

    def __eq__(self, other):
        if not isinstance(other, TBLattice):
            return False
        return self.tb == other.tb and self.bz == other.bz and self.bl == other.bl

    # ---- Print ----

    def __str__(self):
        return str(self.tb)


register_class(TBLattice)
