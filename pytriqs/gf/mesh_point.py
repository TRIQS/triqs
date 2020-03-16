################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2017 by O. Parcollet
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

from past.utils import old_div
class MeshValueGenerator:
    def __init__(self, m):
        self.m = m

    def __iter__(self):
        return (x.value for x in self.m)

class MeshPoint :

    def __init__(self, linear_index, value = None):
        """ None : a mesh point just to use in the G[...] """
        #print "Args", linear_index, value
        self.linear_index, self.value = linear_index, value

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

    def __div__(self, x):
        return old_div(self.value, self._get_val(x))

    def __rdiv__(self, x):
        return old_div(self._get_val(x), self.value)

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

