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

import numpy
import lazy_expressions, descriptor_base
from types import IntType, SliceType, StringType
from _imp_tools import LazyCTX #, IndicesConverter, get_indices_in_dict, py_deserialize
from _gf_plot import PlotWrapperPartialReduce
import warnings

#---------------------   [  ] operator        ------------------------------------------

def __getitem__(self, key):
    """Key is a tuple of index (n1, n2) as defined at construction"""
    if len(key) !=2: raise KeyError, "[ ] must be given two arguments"
    sl1, sl2 = key
    if type(sl1) == StringType and type(sl2) == StringType:
        # Convert the indices to integer
        indices_converter = [ IndicesConverter(self.indicesL), IndicesConverter(self.indicesR)]
        sl1, sl2 =  [ indices_converter[i].convertToNumpyIndex(k) for i, k in enumerate(key) ]
    if type (sl1) != slice: sl1 = slice (sl1, sl1+1)
    if type (sl2) != slice: sl2 = slice (sl2, sl2+1)
    return self.__class__(indicesL = list(self.indicesL)[sl1],
                          indicesR = list(self.indicesR)[sl2],
                          name = self.name,
                          mesh = self.mesh,
                          data = self.data[:,sl1,sl2],
                          tail = self.tail._make_slice(sl1, sl2))

def __setitem__(self, key, val):
    g = self.__getitem__(key)
    g << val

