# Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2023 Simons Foundation
# Copyright (c) 2023 Hugo U.R. Strand
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
# Authors: Michel Ferrero, Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell


r"""Protocol classes for Green's function descriptors.

A *descriptor* is a small object that, when called with a Green's
function ``G``, fills ``G.data`` in place. Descriptors are used by
:meth:`~triqs.gfs.gf.Gf.__lshift__` and :meth:`~triqs.gfs.block_gf.BlockGf.__lshift__` 
so that

    >>> g << SemiCircular(half_bandwidth=1.0)

assigns the closed-form expression of the corresponding model Green's
function to ``g``.

Concrete descriptors are defined in :mod:`triqs.gfs.descriptors`; this
module provides the base classes and the special placeholders
:data:`~triqs.gfs.descriptor_base.Omega` / :data:`~triqs.gfs.descriptor_base.iOmega_n`.
"""

import numpy
from math import *
from .lazy_expressions import LazyExprTerminal, LazyExpr, transform
from triqs.mesh import MeshImFreq, MeshDLRImFreq, MeshReFreq, MeshReFreqPts, MeshReFreqLog

class LazyCTX:
    """Evaluation context used by :meth:`~triqs.gfs.gf.Gf.__lshift__`.

    Wraps a target :class:`~triqs.gfs.gf.Gf` and provides ``__call__``
    to reduce a descriptor to the concrete Green's function obtained by
    applying it to a fresh copy of the target.

    Parameters
    ----------
    G : Gf
        Target on which descriptors will be evaluated.

    Attributes
    ----------
    G : Gf
        The captured target Green's function.
    """

    def __init__ (self, G):
        self.G = G

    def _is_compatible_for_ops(self, g):
        """Whether ``g`` shares a compatible mesh with the captured target.

        Parameters
        ----------
        g : Gf
            Other Green's function.

        Returns
        -------
        bool
            ``True`` when the two meshes are identical or compare equal.
        """
        m1,m2  = self.G.mesh, g.mesh
        return m1 is m2 or m1 == m2

    def __eq__ (self, y):
        """Two contexts are equal iff their captured targets are mesh-compatible.

        Parameters
        ----------
        y : object
            Other context to compare against.

        Returns
        -------
        bool
        """
        return isinstance(y, self.__class__) and self._is_compatible_for_ops(y.G)

    def __call__ (self, x):
        """Reduce a leaf of a lazy expression.

        If ``x`` is a descriptor, evaluate it on a fresh copy of the
        captured target and return the result; otherwise return ``x``
        unchanged (it is treated as a constant).

        Parameters
        ----------
        x : object
            Lazy-expression leaf payload.

        Returns
        -------
        Gf or object
            Reduced value.
        """
        if not isinstance(x, descriptor_base.Base): return x
        tmp = self.G.copy()
        x(tmp)
        return tmp


def is_lazy(y):
    """Whether ``y`` is a descriptor or a lazy expression.

    Parameters
    ----------
    y : object
        Value to test.

    Returns
    -------
    bool
        ``True`` iff ``y`` is an instance of :class:`~triqs.gfs.descriptor_base.Omega_`,
        :class:`~triqs.gfs.lazy_expressions.LazyExpr` or
        :class:`~triqs.gfs.lazy_expressions.LazyExprTerminal`.
    """
    return  isinstance(y,(Omega_, LazyExpr, LazyExprTerminal))

def is_scalar(x):
    """Whether ``x`` is a numerical scalar or numpy array.

    Parameters
    ----------
    x : object
        Value to test.

    Returns
    -------
    bool
        ``True`` for built-in / numpy ints, floats, complex numbers
        and numpy arrays.
    """
    return type(x) in [ type(1), type(1.0), type(1j), numpy.ndarray, int, numpy.int_, numpy.int8, numpy.int16, numpy.int32, float, numpy.float32, numpy.float64, complex, numpy.complex64, numpy.complex128 ]

def convert_scalar_to_const(expr):
  """Lift scalars inside a lazy expression to :class:`~triqs.gfs.descriptor_base.Const` 
  descriptors.

  Used by :meth:`~triqs.gfs.gf.Gf.__lshift__` so that ``g << 2.0`` and
  ``g << iOmega_n + 0.5`` are handled by the descriptor machinery.

  Parameters
  ----------
  expr : LazyExpr
      Expression that may contain raw scalar leaves.

  Returns
  -------
  LazyExpr
      Equivalent expression with every scalar wrapped in a
      :class:`~triqs.gfs.descriptor_base.Const`.
  """

  # if the expression is a pure scalar, replace it by Const
  t = expr.get_terminal()
  if is_scalar(t): return LazyExpr( Const(t) )

  # otherwise: replace all scalar appearing in +/- operations by Const
  def act (tag, childs):
        """Wrap scalar children of an additive node in 
        :class:`~triqs.gfs.descriptor_base.Const`.

        Used as the node callback for :func:`~triqs.gfs.lazy_expressions.transform`.

        Parameters
        ----------
        tag : str
            Node tag (only ``"+"`` and ``"-"`` trigger rewriting).
        childs : list of LazyExpr
            Children of the node; rewritten in place.

        Returns
        -------
        tuple of (str, list of LazyExpr)
            The (possibly rewritten) ``(tag, childs)`` pair.
        """
        if tag in ["+", "-"]:
            for n,c in enumerate(childs):
                t = c.get_terminal()
                if is_scalar(t): childs[n] =  Const (t)
        return (tag,childs)

  return transform(expr, act)

class Base (LazyExprTerminal):
    r"""Base class for every Green's function descriptor.

    Stores the constructor's keyword arguments as attributes. Subclasses
    implement ``__call__(self, G)`` to fill the data of the target
    Green's function ``G`` in place and return it.

    Parameters
    ----------
    **kargs
        Configuration values that subclasses will read back from
        ``self`` during :meth:`~triqs.gfs.descriptor_base.Base.__call__`.

    Notes
    -----
    A descriptor is a :class:`~triqs.gfs.lazy_expressions.LazyExprTerminal`,
    so descriptors can be combined with ``+``, ``-``, ``*``, ``/`` and scalars to form
    :class:`~triqs.gfs.lazy_expressions.LazyExpr` trees evaluated on the
    left-hand side of ``<<``.
    """
    def __init__(self,**kargs):
        self.__dict__.update(kargs)

# The Base for any descriptor taking a BlockGf
class BaseBlock(Base):
    r"""Block-aware descriptor base.

    For a single-:class:`~triqs.gfs.gf.Gf` target this behaves like
    :class:`~triqs.gfs.descriptor_base.Base`. For a :class:`~triqs.gfs.block_gf.BlockGf` 
    target the descriptor builds one sub-descriptor per block, exposed through
    iteration so that :meth:`~triqs.gfs.block_gf.BlockGf.__lshift__` can dispatch them
    block-wise.

    Parameters
    ----------
    G : Gf or BlockGf
        Reference Green's function the descriptor will eventually act
        on. Captured to detect the block-target case.
    *args, **kw
        Forwarded to the per-block sub-descriptor constructor.

    Attributes
    ----------
    G : Gf or BlockGf
        Captured target (inherited from :class:`~triqs.gfs.descriptor_base.Base`).
    descriptor_iter : generator of BaseBlock
        Lazily-constructed per-block sub-descriptors. Only present
        when ``G`` is a :class:`~triqs.gfs.block_gf.BlockGf`; see
        :meth:`~triqs.gfs.descriptor_base.BaseBlock.is_block_descriptor`.
    """
    def __init__ (self, G, *args, **kw):
        Base.__init__(self, G = G)
        if self.is_block_descriptor():
            self.descriptor_iter = (self.__class__(g, *args, **kw) for i,g in G)

    def is_block_descriptor(self):
        """Whether the captured target is a :class:`~triqs.gfs.block_gf.BlockGf`.

        Returns
        -------
        bool
        """
        return self.G.__class__.__name__ == 'BlockGf'

    def __iter__ (self):
        """Yield the per-block sub-descriptors.

        Yields
        ------
        BaseBlock
            One sub-descriptor per block, in block order.
        """
        return self.descriptor_iter

#########################################################################

class Function (Base):
    r"""Descriptor wrapping a Python callable.

    When applied to a Green's function with mesh values
    :math:`\{x_i\}`, sets ``G.data[i] = function(x_i)`` for every
    mesh point.

    Parameters
    ----------
    function : callable
        A function ``f(mesh_value) -> scalar or numpy.ndarray`` matching
        the target's ``target_shape``.

    Attributes
    ----------
    function : callable
        The stored function ``f``, accessible as an attribute.

    Examples
    --------
    >>> from triqs.gfs.descriptor_base import Function
    >>> g << Function(lambda w: 1.0 / (w - 0.5))
    """
    def __init__ (self, function):
        Base.__init__(self, function=function)

    def __call__(self,G):
        """Fill ``G.data`` with ``function`` evaluated at every mesh point.

        Parameters
        ----------
        G : Gf
            Target Green's function (modified in place).

        Returns
        -------
        Gf
            ``G``.

        Raises
        ------
        RuntimeError
            If the stored ``function`` is not callable, or raises while
            being evaluated.
        """
        if not(callable(self.function)): raise RuntimeError("GFInitializer.Function: f must be callable")
        res = G.data[...]
        try:
            for n,om in enumerate(G.mesh): res[n,...] = self.function(om.value)
        except:
            print("The given function has a problem...")
            raise
        return G

#########################################################################

class Const(Base):
    r"""Constant descriptor.

    Fills the target Green's function with a constant matrix (or
    scalar) on a frequency mesh. Used internally by
    :func:`~triqs.gfs.descriptor_base.convert_scalar_to_const` to lift scalar leaves of a
    lazy expression.

    Parameters
    ----------
    C : scalar or numpy.ndarray
        Constant value. A scalar fills the diagonal of a square target
        space; a 2D array must match the target shape.
    """
    def __init__ (self, C):
        Base.__init__(self, C=C)

    def __call__(self,G):
        """Fill ``G.data`` with the stored constant on every mesh point.

        Parameters
        ----------
        G : Gf
            Target Green's function on a frequency mesh (modified in
            place).

        Returns
        -------
        Gf
            ``G``.

        Raises
        ------
        TypeError
            If ``G.mesh`` is not a frequency mesh.
        RuntimeError
            If the stored constant has a shape incompatible with
            ``G.target_shape``.
        """
        C = self.C
        if type(G.mesh) not in [MeshImFreq, MeshDLRImFreq, MeshReFreq, MeshReFreqPts, MeshReFreqLog]:
            raise TypeError("This initializer is only correct in frequency")

        if not isinstance(C,numpy.ndarray) and G.target_rank > 0:
            assert G.target_shape[0]==G.target_shape[1], "Const only applies to square G"
            C = C*numpy.identity(G.target_shape[0])
        if G.target_rank > 0 and C.shape != (G.target_shape[0],G.target_shape[1]): raise RuntimeError("Size of constant incorrect")

        Function(lambda om: C)(G)
        return G

#########################################################################

class Omega_(Base):
    r"""Frequency-variable placeholder descriptor.

    Implements the function :math:`\omega \mapsto \omega` (real or
    Matsubara) acting on the diagonal of the target space. Two
    module-level instances, :data:`~triqs.gfs.descriptor_base.Omega` and 
    :data:`~triqs.gfs.descriptor_base.iOmega_n`, are
    exposed for use in lazy expressions; they are interchangeable
    aliases and the choice between them is purely a matter of
    readability.

    Notes
    -----
    Only frequency meshes (:class:`~triqs.mesh.meshes.MeshImFreq`, 
    :class:`~triqs.mesh.meshes.MeshDLRImFreq`,
    :class:`~triqs.mesh.meshes.MeshReFreq`, :class:`~triqs.mesh.meshes.MeshReFreqPts`,
    :class:`~triqs.mesh.meshes.MeshReFreqLog`) are supported; applying this descriptor to
    an imaginary-time mesh raises :class:`TypeError`.
    """

    def __str__(self):
        """Return the string ``"Omega"``.

        Returns
        -------
        str
        """
        return "Omega"

    def __call__(self,G):
        """Fill ``G.data`` with ``omega * I`` on every frequency mesh point.

        Parameters
        ----------
        G : Gf
            Target Green's function on a frequency mesh (modified in
            place).

        Returns
        -------
        Gf
            ``G``.

        Raises
        ------
        TypeError
            If ``G.mesh`` is not a frequency mesh.
        """
        if type(G.mesh) not in [MeshImFreq, MeshDLRImFreq, MeshReFreq, MeshReFreqPts, MeshReFreqLog]:
            raise TypeError("This initializer is only correct in frequency")

        Id = 1. if G.target_rank == 0 else numpy.identity(G.target_shape[0])

        for n,om in enumerate(G.mesh): G.data[n,...] = om*Id
        return G

##########################################################################

#: Frequency placeholder for use in lazy expressions (real-frequency naming).
Omega = Omega_()

#: Frequency placeholder for use in lazy expressions (Matsubara naming).
#: Aliased to :data:`~triqs.gfs.descriptor_base.Omega`; pick whichever reads better for the mesh at hand.
iOmega_n = Omega_()

##########################################################################
