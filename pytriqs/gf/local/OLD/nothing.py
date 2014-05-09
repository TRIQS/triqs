
################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011-2012 by M. Ferrero, O. Parcollet
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

class Nothing (object):

    def __init__(self): pass
    def __getitem__(self, key): return Nothing()
    def __setitem__(self, key, val): pass
    def __iadd__(self, arg): return Nothing()
    def __add__(self, y): return Nothing()
    def __radd__(self, y): return Nothing()
    def __isub__(self, arg): return Nothing()
    def __sub__(self, y): return Nothing()
    def __rsub__(self, y): return Nothing()
    def __imul__(self, arg): return Nothing()
    def __mul__(self, arg): return Nothing()
    def __rmul__(self, x): return Nothing()
    def __idiv__(self, arg): return Nothing()
    def __div__(self, arg): return Nothing()
    def _make_slice(self, sl1, sl2): return Nothing()
    def invert(self): pass
    def transpose(self): return Nothing()
    def conjugate(self): return Nothing()
    def copy(self): return Nothing()
