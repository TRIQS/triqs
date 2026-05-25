# Copyright (c) 2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2017 Centre national de la recherche scientifique (CNRS)
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
# Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

"""Lightweight Python representation of a single point on a TRIQS mesh."""


class MeshValueGenerator:
    """Iterator adaptor yielding the value of each mesh point of a given mesh.

    Iterating over ``MeshValueGenerator(m)`` is equivalent to
    ``(mp.value for mp in m)``. The class exists as a small helper to
    expose the values of a mesh under a stable, picklable name; it is
    re-exported from :mod:`triqs.gfs` for use by the plotting and
    high-level Green's function code.

    Parameters
    ----------
    m : mesh
        Any object iterable over :class:`MeshPoint` instances (or any
        object exposing a ``.value`` attribute on each element).

    Attributes
    ----------
    m : mesh
        The wrapped mesh, stored verbatim.
    """

    def __init__(self, m):
        self.m = m

    def __iter__(self):
        """Iterate over :attr:`m`, yielding ``mp.value`` for each mesh point.

        Returns
        -------
        generator
            Generator of :attr:`MeshPoint.value` over :attr:`m`.
        """
        return (x.value for x in self.m)

class MeshPoint :
    r"""A single point on a TRIQS mesh.

    A mesh point bundles its mesh-native index :math:`n`, its data 
    index :math:`d`, the hash of the parent mesh and -- for most meshes 
    -- the physical value at the point.

    Arithmetic and casting are forwarded to :attr:`value`, so a
    :class:`MeshPoint` can be used in numerical expressions in place of
    its underlying scalar / vector.

    Parameters
    ----------
    index : object
        Mesh-native index :math:`n`.
    data_index : int
        Data index :math:`d`.
    mesh_hash : int
        Hash of the parent mesh.
    value : object, optional
        Physical value at the point. Defaults to ``None``.
    weight : object, optional
        Optional integration weight associated with the point. 
        Defaults to ``None``.

    Attributes
    ----------
    index : object
        Mesh-native index :math:`n`.
    data_index : int
        Data index :math:`d`.
    mesh_hash : int
        Hash of the parent mesh
    value : object
        Physical value or ``None``.
    weight : object
        Integration weight or ``None``.
    real : object
        Real part of :attr:`value`.
    imag : object
        Imaginary part of :attr:`value`.
    """

    def __init__(self, index, data_index, mesh_hash, value = None, weight = None):
        self.index = index
        self.data_index = data_index
        self.mesh_hash = mesh_hash
        self.value = value
        self.weight = weight

    def _get_val(self, x):
        """Unwrap ``x`` to a plain value: return ``x.value`` if available, otherwise ``x``.

        Parameters
        ----------
        x : object
            Operand of an arithmetic expression. May be another
            :class:`MeshPoint`, a scalar, an array, etc.

        Returns
        -------
        object
            ``x.value`` if ``x`` exposes a ``value`` attribute, ``x``
            itself otherwise.
        """
        return (x.value if hasattr(x, 'value') else x)

    def __add__(self, x):
        """Addition forwarded to :attr:`value`.

        Parameters
        ----------
        x : object
            Right-hand side operand.

        Returns
        -------
        object
            ``self.value + _get_val(x)``.
        """
        return self.value + self._get_val(x)

    def __radd__(self, x):
        """Right-hand addition.

        Parameters
        ----------
        x : object
            Left-hand side operand.

        Returns
        -------
        object
            ``_get_val(x) + self.value``.
        """
        return self._get_val(x) + self.value

    def __sub__(self, x):
        """Subtraction forwarded to :attr:`value`.

        Parameters
        ----------
        x : object
            Right-hand side operand.

        Returns
        -------
        object
            ``self.value - _get_val(x)``.
        """
        return self.value - self._get_val(x)

    def __rsub__(self, x):
        """Right-hand subtraction.

        Parameters
        ----------
        x : object
            Left-hand side operand.

        Returns
        -------
        object
            ``_get_val(x) - self.value``.
        """
        return self._get_val(x) - self.value

    def __neg__(self):
        """Negation.

        Returns
        -------
        object
            ``-self.value``.
        """
        return - self.value

    def __mul__(self, x):
        """Multiplication forwarded to :attr:`value`.

        Parameters
        ----------
        x : object
            Right-hand side operand.

        Returns
        -------
        object
            ``self.value * _get_val(x)``.
        """
        return self.value * self._get_val(x)

    def __rmul__(self, x):
        """Right-hand multiplication.

        Parameters
        ----------
        x : object
            Left-hand side operand.

        Returns
        -------
        object
            ``_get_val(x) * self.value``.
        """
        return self._get_val(x) * self.value

    def __truediv__(self, x):
        """Division forwarded to :attr:`value`.

        Parameters
        ----------
        x : object
            Divisor.

        Returns
        -------
        object
            ``self.value / _get_val(x)``.
        """
        return self.value / self._get_val(x)

    def __rtruediv__(self, x):
        """Right-hand division.

        Parameters
        ----------
        x : object
            Dividend.

        Returns
        -------
        object
            ``_get_val(x) / self.value``.
        """
        return self._get_val(x) / self.value

    def __call__(self, *args, **kw):
        """Forward the call to :attr:`value`.

        Parameters
        ----------
        *args, **kw
            Arguments passed through to ``self.value(*args, **kw)``.

        Returns
        -------
        object
            Result of calling :attr:`value`.
        """
        return self.value(*args, **kw)

    def __getitem__(self, *args):
        """Forward item access to :attr:`value`.

        Parameters
        ----------
        *args
            Index / slice arguments passed through to ``self.value.__getitem__``.

        Returns
        -------
        object
            The selected element(s) of :attr:`value`.
        """
        return self.value.__getitem__(*args)

    def __float__(self):
        """Cast to :class:`float`.

        Returns
        -------
        float
            ``float(self.value)``.
        """
        return float(self.value)

    def __int__(self):
        """Cast to :class:`int`.

        Returns
        -------
        int
            ``int(self.value)``.
        """
        return int(self.value)

    def __complex__(self):
        """Cast to :class:`complex`.

        Returns
        -------
        complex
            ``complex(self.value)``.
        """
        return complex(self.value)

    def __str__(self):
        """Human-readable string ``mesh_point(index = ..., data_index = ..., value = ...)``.

        Returns
        -------
        str
            String describing the mesh point.
        """
        return "mesh_point(index = %s, data_index = %s, value = %s)"%(self.index, self.data_index, self.value)

    @property
    def real(self):
        """Real part of :attr:`value`.

        Returns
        -------
        object
            ``self.value.real``.
        """
        return self.value.real

    @property
    def imag(self):
        """Imaginary part of :attr:`value`.

        Returns
        -------
        object
            ``self.value.imag``.
        """
        return self.value.imag
