import math
import numpy
from operator import add, neg
from triqs.gf import MeshPoint

class LatticePoint:
    r""" TRIQS Lattice Point

    Castable to array of double, arithmetic operations implemented by cast

    Parameters
    ----------
    index: numpy array of int
        The lattice point index
    lattice: BravaisLattice
        The underlying bravais lattice

    Attributes
    ----------
    index: numpy array of int
        The lattice point index
    lattice: BravaisLattice
        The underlying bravais lattice
    """

    def __init__(self, index, lattice):
        self.index = index
        self.lattice = lattice
        self._value = None

    @property
    def value(self):
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
