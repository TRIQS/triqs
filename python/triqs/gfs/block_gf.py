# Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2023 Simons Foundation
# Copyright (c) 2017 Hugo U.R. Strand
# Copyright (c) 2014 Igor Krivenko
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
# Authors: Weh Andreas, Thomas Ayral, Michel Ferrero, Hartmut Hafermann, Alexander Hampel, Jonathan Karp, Igor Krivenko, Olivier Parcollet, Priyanka Seth, Hugo U. R. Strand, Nils Wentzell

r"""Block-diagonal Green's function container (:class:`~triqs.gfs.block_gf.BlockGf`)."""

from collections.abc import Sequence

import numpy as np

from .gf import Gf

def fix_gf_struct_type(gf_struct):
    """Normalise ``gf_struct`` to the canonical ``[(str, int), ...]`` form.

    Older TRIQS releases accepted a ``dict`` mapping block names to
    sizes, or a list of pairs ``(name, [orbital_labels])``. This helper
    converts both to the current ``[(name, linear_size), ...]``
    representation, printing a warning to ``stdout``.

    Parameters
    ----------
    gf_struct : dict, or list of pairs
        Legacy or canonical block-structure description.

    Returns
    -------
    list of (str, int)
        Canonical block structure.
    """
    if isinstance(gf_struct,dict):
        print("WARNING: gf_struct should be a list of pairs [(str,int), ...], not a dict. Converting...")
        return list(gf_struct.items())

    if isinstance(gf_struct[0][1],list):
        print("WARNING: gf_struct should be a list of pairs '(str,int)' containing the block-name and its linear size",
              "and not a list of pairs '(str,[str or int,...])'. Converting...")
        return[(k, len(v)) for k, v in gf_struct]

    return gf_struct

def call_factory_from_dict(cl,name, dic):
    """Factory used by HDF5 IO. Forwards to ``cl.__factory_from_dict__(name, dic)``.

    Parameters
    ----------
    cl : type
        Target class to instantiate.
    name : str
        Name passed through to the factory.
    dic : dict
        Payload deserialised from HDF5.

    Returns
    -------
    object
        Instance of ``cl`` built from ``dic``.
    """
    return cl.__factory_from_dict__(name, dic)

class BlockGf:
    r"""Block-diagonal Green's function.

    A :class:`~triqs.gfs.block_gf.BlockGf` is an **ordered**, named collection of
    :class:`~triqs.gfs.gf.Gf` blocks sharing the same mesh and underlying type — for
    instance one block per spin or one block per symmetry sector. Block
    access uses either the string block name (``g['up']``) or the
    positional integer index (``g[0]``). Iteration yields ``(name,
    block)`` pairs in construction order.

    Three keyword-only constructor patterns are supported (see
    Examples):

    1. ``BlockGf(name_list=..., block_list=..., make_copies=False, name='G')``
       — explicit list of blocks.
    2. ``BlockGf(mesh=..., gf_struct=..., target_rank=2, name='G')``
       — build matrix-valued blocks from a mesh and block structure.
    3. ``BlockGf(name_block_generator=..., make_copies=False, name='G')``
       — iterable of ``(name, block)`` pairs.

    Parameters
    ----------
    name_list : list of str, optional
        Block names, e.g. ``['up', 'dn']``. Pattern 1 only. Defaults to
        ``['0', '1', ...]`` when ``block_list`` is provided.
    block_list : list of Gf, optional
        One :class:`~triqs.gfs.gf.Gf` per block. Pattern 1 only. All 
        blocks must have the same Python type.
    mesh : Mesh, optional
        Common mesh for every block. Pattern 2 only.
    gf_struct : list of (str, int), optional
        ``(block_name, linear_size)`` pairs. Pattern 2 only.
    target_rank : int, optional
        Rank of the target space of each block. Pattern 2 only.
        Default ``2`` (matrix-valued).
    name_block_generator : iterable of (str, Gf), optional
        Iterable of ``(name, block)`` pairs. Pattern 3 only.
    make_copies : bool, optional
        If ``True``, store deep copies of the blocks; otherwise store
        the references. Default ``False``.
    name : str, optional
        Plot label. Default ``'G'``.

    Attributes
    ----------
    name : str
        Plot label; also used as a prefix for the individual block
        names.
    mesh : Mesh
        Mesh shared by every block.
    beta : float
        Inverse temperature of the first block's mesh, when defined.
    indices : generator of str
        Block names in construction order.
    all_indices : generator
        Tuples ``(block_name, n1, n2)`` over every entry in every block.
    gf_struct : list of (str, int)
        Canonical block structure.
    n_blocks : int
        Number of blocks.
    real : BlockGf
        Block-wise view of the real part.
    imag : BlockGf
        Block-wise view of the imaginary part.

    Notes
    -----
    - All blocks must be of the same Python type (e.g. all :class:`~triqs.gfs.gf.Gf`).
    - The ``<<`` operator broadcasts lazy initializers / Green's
      functions to every block; see :meth:`~triqs.gfs.block_gf.BlockGf.__lshift__`.

    Examples
    --------
    Construct a two-block matrix-valued Matsubara Green's function and
    fill every block with a semicircular DOS:

    >>> from triqs.gfs import BlockGf, SemiCircular
    >>> from triqs.mesh import MeshImFreq
    >>> mesh = MeshImFreq(beta=10.0, statistic='Fermion', n_iw=1024)
    >>> G = BlockGf(mesh=mesh, gf_struct=[('up', 2), ('dn', 2)])
    >>> G << SemiCircular(half_bandwidth=1.0)
    >>> for name, g in G:
    ...     print(name, g.target_shape)
    """

    __array_priority__ = 10000 # Makes sure the operations of this class are applied as priority

    # not implemented as a dictionary since we want to be sure of the order !).
    def __init__(self, **kwargs):
        # see the class docstring for the supported constructor patterns
        # first extract the optional name argument
        self.name = kwargs.pop('name','G')
        self._rename_gf = kwargs.pop('rename_gf',True)

        # Default arguments
        if 'block_list' in kwargs:
            kwargs['make_copies'] = kwargs.get('make_copies', False)
            kwargs['name_list'] = kwargs.get('name_list', [str(i) for i in range(len(kwargs['block_list']))])
        if set(kwargs.keys()) == set(['mesh','gf_struct']):
            kwargs['target_rank'] = 2
        if 'name_block_generator' in kwargs:
            kwargs['make_copies'] = kwargs.get('make_copies', False)

        if set(kwargs.keys()) == set(['name_list','block_list','make_copies']):
            BlockNameList, GFlist = kwargs['name_list'],kwargs['block_list']
            assert all([isinstance(name,str) for name in BlockNameList]), "Error in BlockGf Construction: Block-Names must be Strings"
        elif set(kwargs.keys()) == set(['mesh','gf_struct','target_rank']):
            BlockNameList = []
            GFlist = []
            for bl, bl_size in kwargs['gf_struct']:
                BlockNameList.append(bl)
                assert isinstance(bl, str) and isinstance(bl_size, (int, np.integer)), \
                    "gf_struct should be a list of pairs of bl_name (str) and bl_size (int)"
                if bl_size > 0 and kwargs['target_rank'] > 0:
                    GFlist.append(Gf(mesh=kwargs['mesh'], target_shape=[bl_size]*kwargs['target_rank'], name='G_%s'%bl))
                else:
                    GFlist.append(Gf(mesh=kwargs['mesh'], target_shape=[], name='G_%s'%bl))
        elif set(kwargs.keys()) == set(['name_block_generator','make_copies']):
            BlockNameList,GFlist = list(zip(* kwargs['name_block_generator']))
        else:
            raise RuntimeError("BlockGf construction: error in parameters, see the documentation")

        if kwargs.get('make_copies', False): GFlist = [g.copy() for g in GFlist]

        # First a few checks
        assert GFlist !=[], "Empty list of blocks !"
        for ind in BlockNameList: assert str(ind)[0:2] !='__', "indices should not start with __"
        assert len(set(BlockNameList)) == len(BlockNameList),"Block indices of the Green Function are not unique"
        assert len(BlockNameList) == len(GFlist), "Number of indices and of Green Function Blocks differ"
        assert 'block_names' not in BlockNameList, "'block_names' is a reserved keyword. It is not authorized as a block name ! "

        # All blocks are compatible for binary operation
        # --> correction: All blocks have the same type
        #if not reduce (operator.and_,[ GFlist[0]._is_compatible_for_ops(x) for x in GFlist[1:] ] , True):
        #    raise RuntimeError, "The blocks are not compatible for binary operations: not the same type, same temperature, etc..."
        if len(set([ type(g) for g in GFlist])) != 1:
            raise RuntimeError("BlockGf: All block must have the same type %s"%GFlist)

        # init
        self.__indices,self.__GFlist = BlockNameList,GFlist
        try:
            self.__me_as_dict = dict(self)
        except TypeError:
            raise TypeError("indices are not of the correct type")
        self.__BlockIndexNumberTable = dict( (i,n) for n,i in enumerate(self.__indices) ) # a dict: index -> number of its order

        # Add the name to the G
        if self._rename_gf:
            for i,g in self: g.name = "%s_%s"%(str(self.name),i) if self.name else '%s'%(i,)
        del self._rename_gf

    #------------ copy and construction -----------------------------------------------

    def copy(self, *args):
        """Return an independent deep copy of ``self`` (every block is copied).

        Parameters
        ----------
        *args
            Forwarded to the per-block ``copy()`` method.

        Returns
        -------
        BlockGf
            Independent copy with freshly allocated blocks.
        """
        return self.__class__(name_list=self.__indices[:], block_list=[g.copy(*args) for g in self.__GFlist], make_copies=False)

    def view_selected_blocks(self, selected_blocks):
        """Return a new :class:`~triqs.gfs.block_gf.BlockGf` containing **views** of the named blocks.

        Parameters
        ----------
        selected_blocks : sequence of str
            Block names to expose. Must all exist in ``self``.

        Returns
        -------
        BlockGf
            New container holding view-references to the requested
            blocks, in their original order.
        """
        for b in selected_blocks: assert b in self.__indices,"Selected Blocks must be existing blocks"
        return self.__class__ ( name_block_generator = [(n,g) for n,g in self if n in selected_blocks ],make_copies=False)

    def copy_selected_blocks(self, selected_blocks):
        """Return a new :class:`~triqs.gfs.block_gf.BlockGf` containing **deep copies** of the named blocks.

        Parameters
        ----------
        selected_blocks : sequence of str
            Block names to copy.

        Returns
        -------
        BlockGf
            New container with independent copies of the requested
            blocks.
        """
        for b in selected_blocks: assert b in self.__indices,"Selected Blocks must be existing blocks"
        return self.__class__ ( name_block_generator = [(n,g) for n,g in self if n in selected_blocks ],make_copies=True)

    def copy_from(self, G2):
        """Copy the data of ``G2`` into ``self`` block by block.

        Parameters
        ----------
        G2 : BlockGf
            Source container. Must have the same number of blocks in
            the same order and identical per-block ``target_shape``.

        Raises
        ------
        RuntimeError
            If any pair of blocks has incompatible target shape.
        """
        assert isinstance(G2, BlockGf)
        for (i,g),(i2,g2) in zip(self,G2):
           if  (g.target_shape[0],g.target_shape[1]) != (g2.target_shape[0],g2.target_shape[1]):
               raise RuntimeError("Blocks %s and %s of the Green Function do have the same dimension"%(i1,i2))
        for (i,g),(i2,g2) in zip(self,G2): g.copy_from(g2)

     #--------------  Iterators -------------------------

    def __iter__(self):
        """Iterate as ``(block_name, block_gf)`` pairs in construction order.

        Yields
        ------
        tuple of (str, Gf)
            ``(name, block)`` pairs.
        """
        return zip(self.__indices,self.__GFlist)

    #---------------------------------------------------------------------------------

    def _first(self):
        return self.__GFlist[0]

    def __len__(self):
        """Number of blocks.

        Returns
        -------
        int
            ``len(self.__GFlist)``.
        """
        return len(self.__GFlist)

    #---------------- Properties -------------------------------------------

    # Deprecated. Left for backward compatibility ?? but
    # it would not work for Legendre
    @property
    def mesh(self):
        """Mesh shared by every block.

        Returns
        -------
        Mesh
            Mesh shared by every block (read from the first block).

        Notes
        -----
        Deprecated; access ``g.mesh`` on individual blocks instead.
        """
        return self._first().mesh

    @property
    def beta(self):
        r"""Inverse temperature :math:`\beta`.

        Returns
        -------
        float
            :math:`\beta`.
        """
        return self._first().beta

    @property
    def indices(self):
        """Block names in construction order.

        Yields
        ------
        str
            Block name.
        """
        for ind in self.__indices:
            yield ind

    @property
    def all_indices(self):
        """Iterate over flat indices ``(block_name, n1, n2)`` for every matrix-valued block.

        Yields
        ------
        tuple of (str, int, int)
            ``(block_name, n1, n2)`` triple.
        """
        for sig, g in self:
            val = g.indices
            for x in val:
                for y in val:
                    yield (sig, x, y)

    @property
    def gf_struct(self):
        """Canonical block structure.

        Returns
        -------
        list of (str, int)
            ``(block_name, linear_size)`` for every block.
        """
        return [(name, g.target_shape[0]) for name, g in self]

    @property
    def n_blocks(self):
        """Number of blocks.

        Returns
        -------
        int
            ``len(self.__GFlist)``.
        """
        return len(self.__GFlist)

    @property
    def real(self):
        """Block-wise view of the real part.

        Returns
        -------
        BlockGf
            New container holding ``g.real`` for each block; the
            ``name`` is prefixed with ``'Re '`` when ``self.name`` is
            non-empty.
        """
        return BlockGf(name_list=self.__indices, block_list=[g.real for g in self.__GFlist], name=("Re " + self.name) if self.name else '')

    @property
    def imag(self):
        """Block-wise view of the imaginary part.

        Returns
        -------
        BlockGf
            New container holding ``g.imag`` for each block; the
            ``name`` is prefixed with ``'Im '`` when ``self.name`` is
            non-empty.
        """
        return BlockGf(name_list=self.__indices, block_list=[g.imag for g in self.__GFlist], name=("Im " + self.name) if self.name else '')


    #----------------------   IO    -------------------------------------

    def __mymakestring(self,x):
        return str(x).replace(' ','').replace('(','').replace(')','').replace("'",'').replace(',','-')

    def save(self, filename, accumulate=False):
        """Save each block to a text file ``filename_<blockname>``.

        Parameters
        ----------
        filename : str
            Common prefix; each block is written to
            ``"{filename}_{block_name}"``.
        accumulate : bool, optional
            Forwarded to the per-block ``save`` method. Default
            ``False``.
        """
        for i,g in self:
            g.save( "%s_%s"%(filename, self.__mymakestring(i)), accumulate)

    def load(self, filename, no_exception = False):
        """Load each block from a text file ``filename_<blockname>``.

        Parameters
        ----------
        filename : str
            Common prefix.
        no_exception : bool, optional
            If ``True``, silently skip blocks whose file cannot be read.
            Default ``False``.
        """
        for i,g in self:
            try:
                g.load( "%s_%s"%(filename,self.__mymakestring(i)))
            except:
                if not(no_exception): raise

    def __reduce__(self):
        return call_factory_from_dict, (self.__class__,self.name, self.__reduce_to_dict__())

    def __reduce_to_dict__(self):
        d = dict(self)
        d['block_names'] = list(self.indices)
        return d

    @classmethod
    def __factory_from_dict__(cls, name, d):
        # indices : for backward compatibility. indices is str repr of the
        # indices list and we need to  drop name ...
        # block_names in str-mapped just to make sure that the key are python str (they could be numpy.string_, see __reduce_to_dict__)
        keys = list(map(str,d.pop('block_names'))) if 'block_names' in d else eval(d.pop('indices'))
        assert (set(keys).issubset(d.keys())), "Reload mismatch: the block_names and the keys in the hdf5 group do not match"
        res = cls(name_list = keys, block_list = [d[k] for k in keys], make_copies=False, name=name)
        return res

    #--------------  Pretty print -------------------------

    def __repr__(self):
        """Multi-line summary listing every block.

        Returns
        -------
        str
            Header line followed by ``repr(g)`` for each block.
        """
        s =  "Green Function %s composed of %d blocks: \n"%(self.name,self.n_blocks)
        #s =  "Green Function %s composed of %d blocks at inverse temperature beta = %s: \n"%(self.name,self.n_blocks,self.beta)
        for i,g in self:
            s += " %s \n"%repr(g)  #"  Block %s: indices %s \n"%(i,self[i].indices)
        return s

    def __str__ (self):
        """Alias for :meth:`~triqs.gfs.block_gf.BlockGf.__repr__`.

        Returns
        -------
        str
        """
        return self.__repr__()

    #--------------  Bracket operator []  -------------------------

    # string access
    def __getitem_str(self,key):
        try:
            g = self.__me_as_dict[key]
        except KeyError:
            raise IndexError("block index '" + repr(key) + "' incorrect. Possible indices are: "+ repr(self.__indices))
        return g

    # integer access
    def __getitem_int(self, key):
        n = self.n_blocks
        # Handle negative indices (Python convention)
        if key < 0:
            key = n + key
        if key < 0 or key >= n:
            raise IndexError(f"block index out of bounds for BlockGf with {n} blocks")
        return self.__GFlist[key]

    def __getitem__(self, key):
        """Access a block by string name or integer position.

        Parameters
        ----------
        key : str or int
            Block name (e.g. ``'up'``) or zero-based block index
            (negative integers count from the end).

        Returns
        -------
        Gf
            The requested block (a reference, not a copy).

        Raises
        ------
        IndexError
            If ``key`` does not name an existing block, or if the
            integer is out of range.
        TypeError
            If ``key`` is neither a ``str`` nor an ``int``.
        """
        if isinstance(key, str):
            return self.__getitem_str(key)
        elif isinstance(key, int):
            return self.__getitem_int(key)
        else:
            raise TypeError(f"BlockGf can be accessed using str or int indices, not {type(key)}")


    def __setitem__(self,key,val):
        """Assign into a block via ``<<``.

        ``G[key] = val`` is equivalent to ``G[key] << val``.
        ``G[:] = val`` is equivalent to ``G << val`` (broadcast to every
        block).

        Parameters
        ----------
        key : str, int or slice
            Block selector. ``slice(None, None, None)`` broadcasts.
        val : Gf, BlockGf, descriptor or scalar
            Value to assign.
        """
        if key == slice(None, None, None): # G[:] = XXX is the same as G << XXX
            self << val
        else:
            self[key] << val

    # -------------- Various operations -------------------------------------

    def __le__(self, other):
        raise RuntimeError(" Operator <= not defined ")

    def __lshift__(self, A):
        r"""Lazy initialization / copy operator (``G << RHS``).

        Parameters
        ----------
        A : BlockGf, descriptor or Gf-compatible
            * If ``A`` is a :class:`~triqs.gfs.block_gf.BlockGf`, copy block-wise.
            * If ``A`` is a block descriptor (one yielding per-block
              descriptors via ``is_block_descriptor()``), apply each
              sub-descriptor to the corresponding block.
            * Otherwise broadcast ``A`` to every block: ``g << A`` for
              each ``g`` in ``self``.

        Returns
        -------
        BlockGf
            ``self`` (so that ``G << RHS`` can be chained).
        """
        if isinstance(A, self.__class__):
            for (i,g) in self: g << A[i]
        elif hasattr(A, "is_block_descriptor") and A.is_block_descriptor():
            for g,a in zip(self.__GFlist, A): g << a
        else:
            for g in self.__GFlist: g << A
        return self

    def __iadd__(self,arg):
        """In-place block-wise addition ``self += arg``.

        Parameters
        ----------
        arg : BlockGf, sequence, or anything accepted by :meth:`~triqs.gfs.gf.Gf.__iadd__`
            Same-shape :class:`~triqs.gfs.block_gf.BlockGf` adds block-wise; a sequence
            adds element-wise per block; otherwise ``arg`` is
            broadcast to every block.

        Returns
        -------
        BlockGf
        """
        if isinstance(arg, self.__class__):
            for (n,g) in self: self[n] += arg[n]
        elif isinstance(arg, Sequence):
            assert len(arg) == len(self.__GFlist), "list of incorrect length"
            for l,g in zip(arg,self.__GFlist): g +=l
        else:
            for i,g in self: g += arg
        return self

    def __add__(self,y):
        """Block-wise addition; returns a new :class:`~triqs.gfs.block_gf.BlockGf`.

        Parameters
        ----------
        y : same as :meth:`~triqs.gfs.block_gf.BlockGf.__iadd__`

        Returns
        -------
        BlockGf
        """
        c = self.copy()
        c += y
        return c

    def __radd__(self,y):
        """Reflected addition ``y + self``.

        Parameters
        ----------
        y : same as :meth:`~triqs.gfs.block_gf.BlockGf.__add__`

        Returns
        -------
        BlockGf
        """
        return self.__add__(y)

    def __isub__(self,arg):
        """In-place block-wise subtraction ``self -= arg``.

        Parameters
        ----------
        arg : BlockGf, sequence, or anything accepted by :meth:`~triqs.gfs.gf.Gf.__isub__`

        Returns
        -------
        BlockGf
        """
        if isinstance(arg, self.__class__):
           for (n,g) in self: self[n] -= arg[n]
        elif isinstance(arg, Sequence):
            assert len(arg) == len(self.__GFlist) , "list of incorrect length"
            for l,g in zip(arg,self.__GFlist): g -=l
        else:
            for i,g in self: g -= arg
        return self

    def __sub__(self,y):
        """Block-wise subtraction; returns a new :class:`~triqs.gfs.block_gf.BlockGf`.

        Parameters
        ----------
        y : same as :meth:`~triqs.gfs.block_gf.BlockGf.__isub__`

        Returns
        -------
        BlockGf
        """
        c = self.copy()
        c -= y
        return c

    def __rsub__(self,y):
        """Reflected subtraction ``y - self``.

        Parameters
        ----------
        y : same as :meth:`~triqs.gfs.block_gf.BlockGf.__sub__`

        Returns
        -------
        BlockGf
        """
        c = (-1)*self.copy()
        c += y
        return c

    def __imul__(self,arg):
        """In-place block-wise multiplication ``self *= arg``.

        Parameters
        ----------
        arg : BlockGf or anything accepted by :meth:`~triqs.gfs.gf.Gf.__imul__`

        Returns
        -------
        BlockGf
        """
        if isinstance(arg, BlockGf):
            for (n,g) in self: self[n] *= arg[n]
        else:
            for i,g in self: g *= arg
        return self

    def __mul__(self,y):
        """Block-wise multiplication; returns a new :class:`~triqs.gfs.block_gf.BlockGf`.

        Parameters
        ----------
        y : BlockGf or anything accepted by :meth:`~triqs.gfs.gf.Gf.__mul__`

        Returns
        -------
        BlockGf
        """
        if isinstance(y, BlockGf):
            name_list = []
            block_list = []
            for (n,g) in self:
                name_list.append(n)
                block_list.append(self[n] * y[n])
            c = BlockGf(name_list=name_list, block_list=block_list)
        else:
            c = self.copy()
            c *= y
        return c

    def __rmul__(self,x):
        """Reflected multiplication ``x * self``.

        Parameters
        ----------
        x : same as :meth:`~triqs.gfs.block_gf.BlockGf.__mul__`

        Returns
        -------
        BlockGf
        """
        return self.__mul__(x)

    def __itruediv__(self,arg):
        """In-place block-wise division ``self /= arg``.

        Parameters
        ----------
        arg : sequence or scalar
            A sequence divides element-wise per block; a scalar is
            broadcast.

        Returns
        -------
        BlockGf
        """
        if isinstance(arg, Sequence):
            assert len(arg) == len(self.__GFlist) , "list of incorrect length"
            for l,g in zip(arg,self.__GFlist): g /=l
        else:
            for i,g in self: self[i] /= arg
        return self

    def __truediv__(self,y):
        """Block-wise division; returns a new :class:`~triqs.gfs.block_gf.BlockGf`.

        Parameters
        ----------
        y : same as :meth:`~triqs.gfs.block_gf.BlockGf.__itruediv__`

        Returns
        -------
        BlockGf
        """
        c = self.copy()
        c /= y
        return c

    def __neg__(self):
        """Unary minus ``-self``; returns a new :class:`~triqs.gfs.block_gf.BlockGf`.

        Returns
        -------
        BlockGf
        """
        c = self.copy()
        c *= -1
        return c
   #-----------------------------plot protocol -----------------------------------

    def _plot_(self, opt_dict):
        """Implement the TRIQS plot protocol by concatenating every block's curves."""
        r = []
        for sig,g in self:
            initial_dict = opt_dict.copy()
            r += g._plot_(initial_dict)
        self.name, name_kept = self.name, opt_dict.pop('name', self.name)
        first_g_name = self.__GFlist[0].name
        ylabel = r[0]['ylabel'].replace(first_g_name, self.name) if first_g_name else self.name
        for dic in r:
           dic['ylabel'] = ylabel # replace the ylabel of the elements to a single ylabel
        self.name = name_kept
        return r

   #--------------------------------------------------------------------------

    def zero(self):
        """Set every block's data to zero, in place."""
        for i, g in self: g.zero()

    def density(self, *args, **kwargs):
        r"""Compute per-block single-particle density matrices.

        Equivalent to :math:`\langle c^\dagger_i c_j \rangle` evaluated
        per block from the diagonal-frequency / equal-time limit of the
        block Green's function.

        Parameters
        ----------
        *args, **kwargs
            Forwarded to :meth:`~triqs.gfs.gf.Gf.density` for each block.

        Returns
        -------
        dict of {str : numpy.ndarray}
            Mapping ``block_name -> density_matrix``.
        """
        return dict((s, g.density(*args, **kwargs)) for s, g in self)

    def total_density(self, *args, **kwargs):
        """Compute the total density summed over all blocks.

        Parameters
        ----------
        *args, **kwargs
            Forwarded to :meth:`~triqs.gfs.gf.Gf.total_density` for each block.

        Returns
        -------
        float or complex
            Sum of ``g.total_density(...)`` over every block.
        """
        return sum([g.total_density(*args, **kwargs) for i, g in self])

    def __check_attr(self, ATTR):
        if not hasattr(self._first(), ATTR):
            raise RuntimeError("The blocks of this Green Function do not possess the %s method" % ATTR)

    def invert(self):
        """Invert each block in place."""
        self.__check_attr("invert")
        for i, g in self: g.invert()

    def inverse(self):
        """Compute the inverse of every block.

        Returns
        -------
        BlockGf
            New container holding the matrix/scalar inverse of every
            block of ``self``.
        """
        self.__check_attr("inverse")
        return self.__class__(name_block_generator=[(n, g.inverse()) for n, g in self], make_copies=False)

    def transpose(self):
        """Transpose every block in target space.

        Returns
        -------
        BlockGf
            New container holding the target-space transpose of every
            block of ``self``.
        """
        self.__check_attr("transpose")
        return self.__class__(name_block_generator=[(n, g.transpose()) for n, g in self], make_copies=False)

    def conjugate(self):
        """Complex-conjugate every block.

        Returns
        -------
        BlockGf
            New container holding the complex conjugate of every block
            of ``self``.
        """
        self.__check_attr("conjugate")
        return self.__class__(name_block_generator=[(n, g.conjugate()) for n, g in self], make_copies=False)

#     def make_real_in_tau(self):
#       """  """
#       self.__check_attr("make_real_in_tau")
#       return self.__class__( name_block_generator = [ (n, g.make_real_in_tau()) for n,g in self], make_copies=False)

#---------------------------------------------------------

from h5.formats import register_class
register_class (BlockGf)
