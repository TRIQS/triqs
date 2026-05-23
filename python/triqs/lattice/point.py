# Copyright (c) 2023 Simons Foundation
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
# Authors: Michel Ferrero, Nils Wentzell

"""Lattice points on a Bravais lattice."""

import math
import numpy
from operator import add, neg
from triqs.mesh import MeshPoint

class LatticePoint:
    """A point on a Bravais lattice, identified by its integer index vector.

    Castable to a numpy array of float (the real-space coordinates in the standard
    basis) and supports arithmetic with other lattice points, mesh points and numpy
    arrays.

    Parameters
    ----------
    index : array-like of int
        The lattice point index.
    lattice : BravaisLattice
        The underlying :class:`BravaisLattice`.

    Attributes
    ----------
    index : array-like of int
        The lattice point index.
    lattice : BravaisLattice
        The underlying :class:`BravaisLattice`.
    value : numpy.ndarray
        Real-space coordinates of this lattice point in the standard basis.
    """
    
    def __init__(self, index, lattice):
        self.index = index
        self.lattice = lattice
        self._value = None

    @property
    def value(self):
        """Real-space coordinates of this lattice point in the standard basis.

        Computed lazily on first access from :attr:`index` and :attr:`lattice`,
        then cached for subsequent accesses.

        Returns
        -------
        numpy.ndarray
            Real-space coordinates of the lattice point.
        """
        if self._value is None: self._value = self.lattice.lattice_to_real_coordinates(self.index)
        return self._value

    def __array__(self):
        return self.value

    def __add__(self, other):
        if isinstance(other, LatticePoint):
            return LatticePoint(list(map(add, self.index, other.index)), self.lattice)
        elif isinstance(other, MeshPoint):
            return self.__add__(other.value)
        else:
            return self.__array__() + other

    def __getitem__(self, key):
        return self.value.__getitem__(key)

    def __neg__(self):
        return LatticePoint(list(map(neg, self.index)), self.lattice)

    def __radd__(self, other):
        return self.__add__(other)

    def __sub__(self, other):
        return self.__add__(other.__neg__())

    def __rsub__(self, other):
        neg = self.__neg__()
        return neg.__add__(other)

    def __mul__(self, other):
        return self.__array__() * other

    def __rmul__(self, other):
        return other * self.__array__()

    def __truediv__(self, other):
        return self.__array__() / other

    def __len__(self):
        return self.lattice.ndim
