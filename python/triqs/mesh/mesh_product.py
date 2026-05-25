# Copyright (c) 2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2017 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020-2023 Simons Foundation
# Copyright (c) 2017 Igor Krivenko
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

"""Cartesian product of TRIQS meshes for multi-variable Green's functions."""

import itertools
from functools import reduce # Valid in Python 2.6+, required in Python 3
import operator
import numpy as np

def call_factory_from_dict(cl, name, l):
    """Reconstruct an instance of ``cl`` from a dict produced by ``__reduce_to_dict__``.

    Helper used by :meth:`MeshProduct.__reduce__` so that pickling and HDF5
    serialisation can round-trip through the same factory.

    Parameters
    ----------
    cl : type
        Target class; must expose a ``__factory_from_dict__`` classmethod.
    name : str
        Optional name forwarded to ``cl.__factory_from_dict__`` (unused by
        :class:`MeshProduct` itself).
    l : dict
        Dictionary of component meshes keyed by ``"MeshComponent{i}"``.

    Returns
    -------
    object
        ``cl.__factory_from_dict__(name, l)``.
    """
    return cl.__factory_from_dict__(name, l)

class MeshProduct:
    """Cartesian product of TRIQS meshes.

    A :class:`MeshProduct` of ``k`` factor meshes describes the set of
    ``k``-tuples of mesh points, one factor per mesh. It is the canonical
    mesh of multi-variable Green's functions in TRIQS (see
    :class:`triqs.gfs.Gf`); iteration yields composite mesh points
    :math:`(p_1, \\ldots, p_k)` via :func:`itertools.product`.

    Parameters
    ----------
    *mlist : mesh objects
        The factor meshes, passed as separate positional arguments
        (variadic). Each must satisfy the TRIQS mesh interface
        (iterable over :class:`MeshPoint`, supports ``copy``,
        ``copy_from`` and ``to_data_index``).

    Attributes
    ----------
    components : tuple of mesh
        Tuple of factor meshes, in the order given at construction.
    rank : int
        Number of factor meshes (``len(self.components)``).
    """

    def __init__(self, *mlist):
        self._mlist = mlist
        self._hdf5_data_scheme_ = 'MeshProduct'

    @property
    def components(self):
        """Tuple of factor meshes that make up the product.

        Returns
        -------
        tuple of mesh
            The factor meshes, in the order given to the constructor.
        """
        return self._mlist

    def size_of_components(self):
        """Size of each factor mesh.

        Returns
        -------
        generator of int
            Generator yielding ``len(m)`` for each factor mesh ``m`` in
            :attr:`components`.
        """
        return (len(x) for x in self._mlist)

    def __getitem__(self, i):
        """Return the ``i``-th factor mesh from :attr:`components`.

        Parameters
        ----------
        i : int
            Position of the factor mesh in :attr:`components`.

        Returns
        -------
        mesh
            The ``i``-th factor mesh.
        """
        return self._mlist[i]

    @property
    def rank(self):
        """Number of factor meshes (alias for ``len(self.components)``).

        Returns
        -------
        int
            The number of factor meshes.
        """
        return len(self._mlist)

    # @property
    # def size(self) :
        # return len(self)

    def __len__(self):
        r"""Total number of points in the product mesh.

        Returns
        -------
        int
            Product of the sizes of the factor meshes,
            :math:`\prod_i |m_i|`.
        """
        return reduce(operator.mul, self.size_of_components(), 1)

    def __eq__(self, other):
        """Equality test comparing the tuples of factor meshes elementwise.

        Parameters
        ----------
        other : MeshProduct
            Right-hand side of the comparison.

        Returns
        -------
        bool
            ``True`` iff each factor mesh of ``self`` equals the
            corresponding factor mesh of ``other``.
        """
        return self._mlist == other._mlist

    def __iter__(self):
        """Iterate over composite mesh points.

        Returns
        -------
        iterator
            Iterator over tuples of mesh points, one per factor mesh,
            produced by :func:`itertools.product` of the factor meshes.
        """
        return itertools.product(*self._mlist)

    def copy(self):
        """Return a deep copy of this product mesh.

        Each factor mesh is copied via its own ``copy`` method.

        Returns
        -------
        MeshProduct
            New :class:`MeshProduct` whose factor meshes are independent
            copies of those of ``self``.
        """
        return self.__class__(*[x.copy() for x in self._mlist])

    def copy_from(self, another):
        """Deep-copy the contents of ``another`` into ``self`` factor by factor.

        Parameters
        ----------
        another : MeshProduct
            Source product mesh. Must have the same rank as ``self``.

        Returns
        -------
        MeshProduct
            Result of the elementwise ``copy_from`` on the factor meshes.

        Raises
        ------
        AssertionError
            If ``another.rank`` does not match ``self.rank``.
        """
        assert self.rank == another.rank, "copy_from requires the same rank for meshes"
        return self.__class__(*[x.copy_from(y) for x,y in zip(self._mlist, another._mlist)])

    def to_data_index(self, index) :
        """Convert a tuple of per-factor indices to a tuple of data indices.

        Each component of ``index`` is forwarded to the ``to_data_index``
        method of the corresponding factor mesh.

        Parameters
        ----------
        index : iterable
            One mesh-native index per factor mesh, in the order of
            :attr:`components`.

        Returns
        -------
        generator
            Generator yielding the per-factor data index of each
            component, in the same order.
        """
        return (x.to_data_index(i) for x,i in zip(self._mlist, index))

    def __repr__(self):
        """REPL representation listing each factor mesh.

        Returns
        -------
        str
            String of the form ``"MeshProduct of :<repr m1>, <repr m2>, ..."``.
        """
        return "MeshProduct of :" + ', '.join(repr(x) for x in self._mlist)

    def __str__(self):
        """Human-readable string joining each factor mesh's ``str`` with commas.

        Returns
        -------
        str
            Concatenation of ``str(m)`` for each factor mesh, separated
            by ``", "``.
        """
        return ', '.join(str(x) for x in self._mlist)

   #-----------------------------  IO  -----------------------------------

    def __reduce__(self):
        """Pickle hook.

        Returns
        -------
        tuple
            ``(call_factory_from_dict, (cls, "", reduced_dict))`` where
            ``reduced_dict`` is the output of :meth:`__reduce_to_dict__`.
        """
        return call_factory_from_dict, (self.__class__, "", self.__reduce_to_dict__())

    def __reduce_to_dict__(self):
        """Serialise to a dict keyed by ``"MeshComponent{i}"`` (used by the HDF5 layer).

        Returns
        -------
        dict
            Mapping ``"MeshComponent{i}" -> m_i`` for each factor mesh.
        """
        return dict (('MeshComponent%s'%i, m) for i,m in enumerate(self._mlist))

    # @classmethod
    # def __factory_from_dict__(cls, l):
        # return cls(*l)

    @classmethod
    def __factory_from_dict__(cls, name, d):
        """Reconstruct a :class:`MeshProduct` from a dict produced by :meth:`__reduce_to_dict__`.

        Parameters
        ----------
        name : str
            Unused (kept for the generic factory signature).
        d : dict
            Dictionary keyed by ``"MeshComponent{i}"`` mapping to each
            factor mesh.

        Returns
        -------
        MeshProduct
            New :class:`MeshProduct` whose factor meshes are taken from
            ``d`` in index order.
        """
        return cls(*(d['MeshComponent%s'%i] for i in range(len(d)))) #.values())


#---------------------------------------------------------

from h5.formats import register_class
register_class (MeshProduct)
