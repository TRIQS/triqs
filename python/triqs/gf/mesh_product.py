# Copyright (c) 2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2017 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2017 Igor Krivenko
# Copyright (c) 2020 Simons Foundation
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
# Authors: Michel Ferrero, Igor Krivenko, Olivier Parcollet, Nils Wentzell

import itertools
from functools import reduce # Valid in Python 2.6+, required in Python 3
import operator
import numpy as np

def call_factory_from_dict(cl, name, l):
    return cl.__factory_from_dict__(name, l)

class MeshProduct:
    """
    The cartesian Mesh product

    Parameters
    ----------
    mlist: list
        The list of Meshes in the MeshProduct
    """

    def __init__(self, *mlist):
        self._mlist = mlist
        self._hdf5_data_scheme_ = 'MeshProduct'

    @property
    def components(self):
        """
        A list of the meshes
        """
        return self._mlist

    def size_of_components(self):
        """
        A tuple with the size of the compomements
        """
        return (len(x) for x in self._mlist)

    def __getitem__(self, i):
        return self._mlist[i]

    @property
    def rank(self):
        return len(self._mlist)

    # @property
    # def size(self) :
        # return len(self)

    def __len__(self):
        """
        Total number of points (product of size of components)
        """
        return reduce(operator.mul, self.size_of_components(), 1)

    def __eq__(self, other):
        """
        Compare with another product mesh
        """
        return self._mlist == other._mlist

    def __iter__(self):
        """
        The composite mesh point
        """
        return itertools.product(*self._mlist)

    def copy(self):
        """
         Deep copy
        """
        return self.__class__(*[x.copy() for x in self._mlist])

    def copy_from(self, another):
        """
         Deep copy
        """
        assert self.rank == another.rank, "copy_from requires the same rank for meshes"
        return self.__class__(*[x.copy_from(y) for x,y in zip(self._mlist, another._mlist)])

    def index_to_linear(self, idx) :
        """
        """
        return (x.index_to_linear(i) for x,i in zip(self._mlist, idx))

    def __repr__(self):
        """ """
        return "MeshProduct of :" + ', '.join(repr(x) for x in self._mlist)

    def __str__(self):
        """ """
        return ', '.join(str(x) for x in self._mlist)

   #-----------------------------  IO  -----------------------------------

    def __reduce__(self):
        return call_factory_from_dict, (self.__class__, "", self.__reduce_to_dict__())

    def __reduce_to_dict__(self):
        return dict (('MeshComponent%s'%i, m) for i,m in enumerate(self._mlist))

    # @classmethod
    # def __factory_from_dict__(cls, l):
        # return cls(*l)

    @classmethod
    def __factory_from_dict__(cls, name, d):
        return cls(*(d['MeshComponent%s'%i] for i in range(len(d)))) #.values())


#---------------------------------------------------------

from h5.formats import register_class
register_class (MeshProduct)

