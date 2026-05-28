# Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2023 Simons Foundation
# Copyright (c) 2016 Igor Krivenko
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
# Authors: Weh Andreas, Michel Ferrero, Alexander Hampel, Jonathan Karp, Igor Krivenko, Nils Wentzell

r"""Two-index block-diagonal Green's function container (:class:`~triqs.gfs.block2_gf.Block2Gf`)."""

from collections.abc import Sequence
from itertools import chain, product

import numpy as np


class Block2Gf:
    r"""Two-index block-diagonal Green's function.

    A :class:`~triqs.gfs.block2_gf.Block2Gf` is a rectangular, named collection of
    :class:`~triqs.gfs.gf.Gf` blocks indexed by a pair ``(bn1, bn2)`` drawn from two —
    possibly distinct — sets of block names. It is the natural
    container for two-particle / four-leg quantities such as
    susceptibilities and vertex functions. Block access uses tuple
    indexing (``g['up', 'dn']``). Iteration yields ``((bn1, bn2),
    block)`` pairs in row-major order.

    Parameters
    ----------
    name_list1 : sequence of hashable
        Block names of the first index.
    name_list2 : sequence of hashable
        Block names of the second index.
    block_list : list of lists of Gf, or callable
        Either a 2D nested list of shape
        ``(len(name_list1), len(name_list2))`` of :class:`~triqs.gfs.gf.Gf` blocks, or
        a callable ``f(bn1, bn2) -> Gf`` invoked for every name pair.
    make_copies : bool, optional
        If ``True``, store deep copies of the blocks; otherwise store
        the references. Default ``False``.
    name : str, optional
        Plot label. Default ``''``.

    Attributes
    ----------
    name : str
        Plot label; also used as a prefix for the individual block
        names.
    indices : generator of (hashable, hashable)
        All ``(bn1, bn2)`` pairs in row-major order.
    indices1 : generator
        First-index block names.
    indices2 : generator
        Second-index block names.
    all_indices : generator
        Tuples ``(bn1, bn2, i, j, ...)`` over every target-space entry
        of every block.
    n_blocks : int
        Total number of blocks (``len(name_list1) * len(name_list2)``).
    real : Block2Gf
        Block-wise view of the real part.
    imag : Block2Gf
        Block-wise view of the imaginary part.

    Notes
    -----
    - All blocks must be of the same Python type (e.g. all :class:`~triqs.gfs.gf.Gf`).
    - The ``<<`` operator broadcasts initializers to every block; see
      :meth:`~triqs.gfs.block2_gf.Block2Gf.__lshift__`.

    Examples
    --------
    Construct a 2x2 block susceptibility on a bosonic Matsubara mesh
    and iterate over the blocks:

    >>> from triqs.gfs import Block2Gf, Gf
    >>> from triqs.mesh import MeshImFreq
    >>> mesh = MeshImFreq(beta=10.0, statistic='Boson', n_iw=64)
    >>> chi = Block2Gf(['up', 'dn'], ['up', 'dn'],
    ...                lambda b1, b2: Gf(mesh=mesh, target_shape=[1, 1]),
    ...                name='chi')
    >>> for (bn1, bn2), g in chi:
    ...     print(bn1, bn2, g.target_shape)
    """

    __array_priority__ = 10000 # Makes sure the operations of this class are applied as priority

    def __init__(self, name_list1, name_list2, block_list, **kwargs):
        # see the class docstring for parameter documentation
        # first extract the optional name argument
        self.name = kwargs.pop('name','')

        rename_gf = kwargs.pop('rename_gf',True)
        make_copies = kwargs.get('make_copies', False)

        # First a few checks
        for name_list, var_name in ((name_list1,'name_list1'),(name_list2,'name_list2')):
            assert name_list, "%s: Empty list of block names" % var_name
            assert len(set(name_list)) == len(name_list), "%s: block names are not unique" % var_name
            for bn in name_list:
                try:
                    hash(bn)
                except:
                    raise RuntimeError("%s: block name '%s' is not hashable" % (var_name,bn))
            for bn in name_list:
                assert not str(bn).startswith('__'), "%s: block names should not start with __" % var_name
            assert 'block_names' not in name_list, "%s: 'block_names' is a reserved keyword. It is not authorized as a block name" % var_name

        self.__indices1, self.__indices2 = tuple(name_list1), tuple(name_list2)
        self.__GFlist = []

        if callable(block_list):
            for bn1 in name_list1:
                self.__GFlist.append([])
                for bn2 in name_list2:
                    self.__GFlist[-1].append(block_list(bn1,bn2).copy() if make_copies else block_list(bn1,bn2))
        elif isinstance(block_list, Sequence):
            try:
                assert len(block_list) == len(name_list1), "incorrect outer size of block_list"
                for g_row in block_list:
                    assert len(g_row) == len(name_list2), "incorrect inner size of block_list"
                    self.__GFlist.append([])
                    for g in g_row:
                        self.__GFlist[-1].append(g.copy() if make_copies else g)
            except TypeError: pass
        else:
            raise RuntimeError("block_list is not understood, see the documentation")

        # All blocks must have the same type
        if len(set([type(g) for g in chain.from_iterable(self.__GFlist)])) != 1:
            raise RuntimeError("Block2Gf: All block must have the same type")

        # Add the name to the G
        if rename_gf:
            for i,g in self: g.name = "%s_%s"%(str(self.name),i) if self.name else '%s'%(i,)

    #------------ copy and construction -----------------------------------------------

    def copy(self, *args):
        """Return an independent deep copy of ``self`` (every block is copied).

        Parameters
        ----------
        *args
            Forwarded to the per-block ``copy()`` method.

        Returns
        -------
        Block2Gf
            Independent copy with freshly allocated blocks.
        """
        return self.__class__(self.__indices1[:], self.__indices2[:],
                              [[g.copy(*args) for g in g_row] for g_row in self.__GFlist], make_copies=False)

    def view_selected_blocks(self, selected_blocks1, selected_blocks2):
        """Return a new :class:`~triqs.gfs.block2_gf.Block2Gf` containing **views** of the named blocks.

        Parameters
        ----------
        selected_blocks1 : sequence of hashable
            First-index block names to expose. Must all exist in ``self``.
        selected_blocks2 : sequence of hashable
            Second-index block names to expose. Must all exist in
            ``self``.

        Returns
        -------
        Block2Gf
            New container holding view-references to the requested
            blocks, in the requested order.
        """
        for bn in selected_blocks1: assert bn in self.__indices1, "selected_blocks1: selected blocks must be existing blocks"
        for bn in selected_blocks2: assert bn in self.__indices2, "selected_blocks2: selected blocks must be existing blocks"
        return self.__class__(selected_blocks1, selected_blocks2, lambda bn1, bn2: self[bn1, bn2], make_copies=False)

    def copy_selected_blocks(self, selected_blocks1, selected_blocks2):
        """Return a new :class:`~triqs.gfs.block2_gf.Block2Gf` containing **deep copies** of the named blocks.

        Parameters
        ----------
        selected_blocks1 : sequence of hashable
            First-index block names to copy.
        selected_blocks2 : sequence of hashable
            Second-index block names to copy.

        Returns
        -------
        Block2Gf
            New container with independent copies of the requested
            blocks.
        """
        for bn in selected_blocks1: assert bn in self.__indices1, "selected_blocks1: selected blocks must be existing blocks"
        for bn in selected_blocks2: assert bn in self.__indices2, "selected_blocks2: selected blocks must be existing blocks"
        return self.__class__(selected_blocks1, selected_blocks2, lambda bn1, bn2: self[bn1, bn2], make_copies=True)

    def copy_from(self, G2):
        """Copy the data of ``G2`` into ``self`` block by block.

        Parameters
        ----------
        G2 : Block2Gf
            Source container. Must have the same shape and identical
            per-block data shape.

        Raises
        ------
        AssertionError
            If any pair of blocks has incompatible data shape.
        """
        assert isinstance(G2, Block2Gf)
        for (i, g), (i2, g2) in zip(self, G2):
            assert g.data.shape == g2.data.shape, "Blocks %s and %s of the Green Function do have the same dimension" % (i, i2)
        for (i, g), (i2, g2) in zip(self, G2): g.copy_from(g2)

     #--------------  Iterators -------------------------

    def __iter__(self):
        """Iterate as ``((bn1, bn2), block_gf)`` pairs in row-major order.

        Yields
        ------
        tuple of ((hashable, hashable), Gf)
            ``((bn1, bn2), block)`` pairs.
        """
        return zip(product(self.__indices1, self.__indices2), chain.from_iterable(self.__GFlist))

    #---------------------------------------------------------------------------------

    def _first(self):
        return self.__GFlist[0][0]

    def __len__(self):
        """Total number of blocks.

        Returns
        -------
        int
            ``len(name_list1) * len(name_list2)``.
        """
        return len(self.__indices1) * len(self.__indices2)

    #---------------- Properties -------------------------------------------

    @property
    def indices(self):
        """Iterate over ``(bn1, bn2)`` pairs in row-major order.

        Yields
        ------
        tuple of (hashable, hashable)
            ``(bn1, bn2)`` block-name pair.
        """
        for ind in product(self.__indices1, self.__indices2):
            yield ind

    @property
    def indices1(self):
        """Iterate over the first-index block names.

        Yields
        ------
        hashable
            First-index block name.
        """
        for ind in self.__indices1:
            yield ind

    @property
    def indices2(self):
        """Iterate over the second-index block names.

        Yields
        ------
        hashable
            Second-index block name.
        """
        for ind in self.__indices2:
            yield ind

    @property
    def all_indices(self):
        """Iterate over flat indices ``(bn1, bn2, i, j, ...)`` for every target-space entry of every block.

        Yields
        ------
        tuple
            ``(bn1, bn2, i, j, ...)`` flat index.
        """
        for sig, g in self:
            # A bit hacky...
            mesh_dim = len(g.mesh.components) if hasattr(g.mesh, 'components') else 1
            target_shape = g.data.shape[mesh_dim:]
            for i in product(*list(map(range, target_shape))):
                yield (sig[0], sig[1]) + i

    @property
    def n_blocks(self):
        """Total number of blocks.

        Returns
        -------
        int
            ``len(name_list1) * len(name_list2)``.
        """
        return len(self)

    @property
    def real(self):
        """Block-wise view of the real part of every block.

        Returns
        -------
        Block2Gf
            New container holding ``g.real`` for each block; the
            ``name`` is prefixed with ``'Re '`` when ``self.name`` is
            non-empty.
        """
        return Block2Gf(name_list1=self.__indices1, name_list2=self.__indices2, block_list=[[g.real for g in g_row] for g_row in self.__GFlist], name=("Re " + self.name) if self.name else '')

    @property
    def imag(self):
        """Block-wise view of the imaginary part of every block.

        Returns
        -------
        Block2Gf
            New container holding ``g.imag`` for each block; the
            ``name`` is prefixed with ``'Im '`` when ``self.name`` is
            non-empty.
        """
        return Block2Gf(name_list1=self.__indices1, name_list2=self.__indices2, block_list=[[g.imag for g in g_row] for g_row in self.__GFlist], name=("Im " + self.name) if self.name else '')


    #----------------------   IO    -------------------------------------

    def __reduce__(self):
        return lambda cl,name,dic: cl.__factory_from_dict__(name, dic), (self.__class__,self.name, self.__reduce_to_dict__())

    def __reduce_to_dict__(self):
        d = {"%s_%s"%bn : g for bn, g in self}
        d['block_names1'] = list(self.__indices1)
        d['block_names2'] = list(self.__indices2)
        return d

    @classmethod
    def __factory_from_dict__(cls, name, d):
        block_names1, block_names2 = tuple(d.pop('block_names1')), tuple(d.pop('block_names2'))
        d.pop('note','')
        expected_keys = ["%s_%s" % bn for bn in product(block_names1, block_names2)]
        assert sorted(expected_keys) == sorted(d.keys()), "Reload mismatch: the indices and the group names do not corresponds"
        res = cls(block_names1, block_names2, lambda bn1, bn2: d["%s_%s"%(bn1,bn2)], make_copies=False, name=name)
        return res

    #--------------  Pretty print -------------------------

    def __repr__(self):
        """Multi-line summary listing every block.

        Returns
        -------
        str
            Header line followed by ``repr(g)`` for each block.
        """
        s =  "Green's Function %s composed of %d 2-index blocks: \n"%(self.name,self.n_blocks)
        for i,g in self:
            s += " %s \n"%repr(g)
        return s

    def __str__(self):
        """Return ``self.name`` when set, otherwise ``repr(self)``.

        Returns
        -------
        str
            ``self.name`` if non-empty, else ``repr(self)``.
        """
        return self.name if self.name else repr(self)

    #--------------  Bracket operator []  -------------------------

    def __getitem__(self, key):
        """Access a single block by name pair.

        Parameters
        ----------
        key : tuple of (hashable, hashable)
            ``(bn1, bn2)`` block name pair.

        Returns
        -------
        Gf
            The requested block (a reference, not a copy).

        Raises
        ------
        IndexError
            If ``key`` does not name an existing block.
        """
        try:
            g = self.__GFlist[self.__indices1.index(key[0])][self.__indices2.index(key[1])]
        except KeyError:
            raise IndexError("Block index '" + repr(key) + "' incorrect. Possible indices are: " + repr(list(product(self.__indices1, self.__indices2))))
        return g

    def __setitem__(self, key, val):
        """Assign into a block via ``<<``.

        ``G[bn1, bn2] = val`` is equivalent to ``G[bn1, bn2] << val``.

        Parameters
        ----------
        key : tuple of (hashable, hashable)
            ``(bn1, bn2)`` block name pair.
        val : Gf, descriptor or scalar
            Value to assign.

        Raises
        ------
        IndexError
            If ``key`` does not name an existing block.
        """
        try:
            g = self.__GFlist[self.__indices1.index(key[0])][self.__indices2.index(key[1])]
        except KeyError:
            raise IndexError("Block index '" + repr(key) + "' incorrect. Possible indices are: " + repr(list(product(self.__indices1, self.__indices2))))
        g << val

    # -------------- Various operations -------------------------------------

    def __lshift__(self, A):
        r"""Lazy initialization / copy operator (``G << RHS``).

        Parameters
        ----------
        A : Block2Gf, or per-block initializer
            - If ``A`` is a :class:`~triqs.gfs.block2_gf.Block2Gf`, copy block-wise.
            - Otherwise broadcast: ``self[bn] << A[bn]`` for every
              block (``A`` must be indexable by name pair).

        Returns
        -------
        Block2Gf
            ``self`` (so that ``G << RHS`` can be chained).
        """
        if isinstance(A, self.__class__):
            for bn, g in self: g << A[bn]
        else:
            for bn, g in self: self[bn] << A[bn]
        return self

    def __iadd__(self, arg):
        """In-place block-wise addition ``self += arg``.

        Parameters
        ----------
        arg : Block2Gf, sequence, or anything accepted by :meth:`~triqs.gfs.gf.Gf.__iadd__`
            Same-shape :class:`~triqs.gfs.block2_gf.Block2Gf` adds block-wise; a sequence
            adds element-wise per block; otherwise ``arg`` is broadcast
            to every block.

        Returns
        -------
        Block2Gf
        """
        if isinstance(arg, self.__class__):
            for bn, g in self: self[bn] += arg[bn]
        elif isinstance(arg, Sequence):
            assert len(arg) == len(self.__GFlist), "list of incorrect length"
            for l, (bn, g) in zip(arg, self.__GFlist): self[bn] += l
        else:
            for bn, g in self: self[bn] += arg
        return self

    def __add__(self, y):
        """Block-wise addition; returns a new :class:`~triqs.gfs.block2_gf.Block2Gf`.

        Parameters
        ----------
        y : same as :meth:`~triqs.gfs.block2_gf.Block2Gf.__iadd__`

        Returns
        -------
        Block2Gf
        """
        c = self.copy()
        c += y
        return c

    def __radd__(self, y):
        """Reflected addition ``y + self``.

        Parameters
        ----------
        y : same as :meth:`~triqs.gfs.block2_gf.Block2Gf.__add__`

        Returns
        -------
        Block2Gf
        """
        return self.__add__(y)

    def __isub__(self, arg):
        """In-place block-wise subtraction ``self -= arg``.

        Parameters
        ----------
        arg : Block2Gf, sequence, or anything accepted by :meth:`~triqs.gfs.gf.Gf.__isub__`

        Returns
        -------
        Block2Gf
        """
        if isinstance(arg, self.__class__):
            for bn, g in self: self[bn] -= arg[bn]
        elif isinstance(arg, Sequence):
            assert len(arg) == len(self.__GFlist), "list of incorrect length"
            for l, (bn, g) in zip(arg, self.__GFlist): self[bn] -= l
        else:
            for bn, g in self: self[bn] -= arg
        return self

    def __sub__(self, y):
        """Block-wise subtraction; returns a new :class:`~triqs.gfs.block2_gf.Block2Gf`.

        Parameters
        ----------
        y : same as :meth:`~triqs.gfs.block2_gf.Block2Gf.__isub__`

        Returns
        -------
        Block2Gf
        """
        c = self.copy()
        c -= y
        return c

    def __rsub__(self, y):
        """Reflected subtraction ``y - self``.

        Parameters
        ----------
        y : same as :meth:`~triqs.gfs.block2_gf.Block2Gf.__sub__`

        Returns
        -------
        Block2Gf
        """
        c = (-1) * self.copy()
        c += y
        return c

    def __imul__(self, arg):
        """In-place block-wise multiplication ``self *= arg``.

        Parameters
        ----------
        arg : Block2Gf, sequence, or anything accepted by :meth:`~triqs.gfs.gf.Gf.__imul__`

        Returns
        -------
        Block2Gf
        """
        if isinstance(arg, self.__class__):
            for bn, g in self: self[bn] *= arg[bn]
        elif isinstance(arg, Sequence):
            assert len(arg) == len(self.__GFlist), "list of incorrect length"
            for l, (bn, g) in zip(arg, self.__GFlist): self[bn] *= l
        else:
            for bn, g in self: self[bn] *= arg
        return self

    def __mul__(self, y):
        """Block-wise multiplication; returns a new :class:`~triqs.gfs.block2_gf.Block2Gf`.

        Parameters
        ----------
        y : Block2Gf or anything accepted by :meth:`~triqs.gfs.gf.Gf.__mul__`

        Returns
        -------
        Block2Gf
        """
        if isinstance(y, self.__class__):
            block_list = []
            for i1, name1 in enumerate(self.__indices1):
                block_list.append([])
                for i2, name2 in enumerate(self.__indices2):
                    block_list[i1].append(self[(name1, name2)] * y[(name1, name2)])
            c = Block2Gf(name_list1=self.__indices1, name_list2=self.__indices2, block_list=block_list)
        else:
            c = self.copy()
            c *= y
        return c

    def __rmul__(self, x):
        """Reflected multiplication ``x * self``.

        Parameters
        ----------
        x : same as :meth:`~triqs.gfs.block2_gf.Block2Gf.__mul__`

        Returns
        -------
        Block2Gf
        """
        return self.__mul__(x)

    def __itruediv__(self, arg):
        """In-place block-wise division ``self /= arg``.

        Parameters
        ----------
        arg : sequence or scalar
            A sequence divides element-wise per block; a scalar is
            broadcast.

        Returns
        -------
        Block2Gf
        """
        if isinstance(arg, Sequence):
            assert len(arg) == len(self.__GFlist), "list of incorrect length"
            for l, (bn, g) in zip(arg, self.__GFlist): self[bn] /= l
        else:
            for bn, g in self: self[bn] /= arg
        return self

    def __truediv__(self, y):
        """Block-wise division; returns a new :class:`~triqs.gfs.block2_gf.Block2Gf`.

        Parameters
        ----------
        y : same as :meth:`~triqs.gfs.block2_gf.Block2Gf.__itruediv__`

        Returns
        -------
        Block2Gf
        """
        c = self.copy()
        c /= y
        return c

    def __neg__(self):
        """Unary minus ``-self``; returns a new :class:`~triqs.gfs.block2_gf.Block2Gf`.

        Returns
        -------
        Block2Gf
        """
        c = self.copy()
        c *= -1
        return c
    #-----------------------------plot protocol -----------------------------------

    def _plot_(self, opt_dict):
        """Implement the TRIQS plot protocol by concatenating every block's curves."""
        r = []
        for bn, g in self:
            initial_dict = opt_dict.copy()
            r += g._plot_(initial_dict)
        self.name, name_kept = self.name, opt_dict.pop('name', self.name)
        first_g_name = self._first().name
        ylabel = r[0]['ylabel'].replace(first_g_name, self.name) if first_g_name else self.name
        for dic in r:
            dic['ylabel'] = ylabel # replace the ylabel of the elements to a single ylabel
        self.name = name_kept
        return r

    #--------------------------------------------------------------------------

    def zero(self):
        """Set every block's data to zero, in place."""
        for i, g in self: g.zero()

    def __check_attr(self, attr):
        if not hasattr(self._first(), attr):
            raise RuntimeError("The blocks of this Green's Function do not possess the %s method" % attr)

#---------------------------------------------------------

from h5.formats import register_class
register_class(Block2Gf)
