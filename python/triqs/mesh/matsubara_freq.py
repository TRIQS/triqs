# Copyright (c) 2022-2023 Simons Foundation
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
# Authors: Michel Ferrero, Dominik Kiese, Nils Wentzell

import math

class MatsubaraFreq:
    r""" TRIQS Matsubara frequency

    Castable to complex, arithmetic operations implemented by cast

    Parameters
    ----------
    n: int
        The Matsubara index
    beta: double
        The inverse temperature
    statistic: str
        The particle statistic, 'Fermion' or 'Boson'

    Attributes
    ----------
    n: int
        The Matsubara index
    beta: double
        The inverse temperature
    statistic: str
        The particle statistic, 'Fermion' or 'Boson'
    """

    def __init__(self, n, beta, statistic="Fermion"):
        self.n = n
        self.beta = beta
        self.statistic = statistic
        self.is_fermion = (True if self.statistic == "Fermion" else False)

    def __complex__(self):
        return 1j * (2*self.n + self.is_fermion) * math.pi / self.beta

    @property
    def index(self):
        return self.n

    @property
    def imag(self):
        return complex(self).imag

    @property
    def real(self):
        return 0.0

    @property
    def value(self):
        return complex(self)

    def __eq__(self, other):
        if isinstance(other, MatsubaraFreq):
            return self.n == other.n and self.beta == other.beta and self.statistic == other.statistic
        else:
            return False

    def __abs__(self):
        return abs(complex(self))

    def __add__(self, other):
        if isinstance(other, MatsubaraFreq):
            if not self.beta == other.beta:
                raise RuntimeError(f"Values of beta do not match: self.beta={self.beta} other.beta={other.beta}")
            is_fermion = self.is_fermion ^ other.is_fermion
            statistic = "Fermion" if is_fermion else "Boson"
            return MatsubaraFreq(self.n + other.n + (self.is_fermion and other.is_fermion), self.beta, statistic)
        else:
            return complex(self) + other

    def __neg__(self):
        return MatsubaraFreq(-self.n - self.is_fermion, self.beta, self.statistic)

    def __radd__(self, other):
        return self.__add__(other)

    def __sub__(self, other):
        return self.__add__(other.__neg__())

    def __rsub__(self, other):
        neg = self.__neg__()
        return neg.__add__(other)

    def __mul__(self, other):
        return complex(self) * other

    def __rmul__(self, other):
        return other * complex(self)

    def __truediv__(self, other):
        return complex(self) / other

    def __rtruediv__(self, other):
        return other / complex(self)

    def __pow__(self, other):
        return complex(self)**other

    def __str__(self):
        return f"MatsubaraFreq(n: {self.n}, beta: {self.beta}, statistic: {self.statistic})"

    def __repr__(self):
        return self.__str__()
