# Copyright (c) 2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2017 Centre national de la recherche scientifique (CNRS)
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


class MeshValueGenerator:
    def __init__(self, m):
        self.m = m

    def __iter__(self):
        return (x.value for x in self.m)

class MeshPoint :

    def __init__(self, linear_index, index = None, value = None, weight = None):
        """ None : a mesh point just to use in the G[...] """
        self.linear_index = linear_index
        self.index = index
        self.value = value
        self.weight = weight

    def _get_val(self, x):
        return (x.value if hasattr(x, 'value') else x)

    def __add__(self, x):
        return self.value + self._get_val(x)

    def __radd__(self, x):
        return self._get_val(x) + self.value

    def __sub__(self, x):
        return self.value - self._get_val(x)

    def __rsub__(self, x):
        return self._get_val(x) - self.value

    def __mul__(self, x):
        return self.value * self._get_val(x)

    def __rmul__(self, x):
        return self._get_val(x) * self.value

    def __truediv__(self, x):
        return self.value / self._get_val(x)

    def __rtruediv__(self, x):
        return self._get_val(x) / self.value

    def __call__(self, *args, **kw):
        return self.value(*args, **kw)

    def __getitem__(self, *args):
        return self.value.__getitem__(*args)

    def __float__(self):
        return float(self.value)

    def __int__(self):
        return int(self.value)

    def __complex__(self):
        return complex(self.value)

    def __str__(self):
        return "mesh_point(linear_index = %s, value = %s)"%(self.linear_index, self.value)

    @property
    def real(self):
        return self.value.real

    @property
    def imag(self):
        return self.value.imag

