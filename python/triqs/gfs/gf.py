# Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2023 Simons Foundation
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
# Authors: Michel Ferrero, Olivier Gingras, Alexander Hampel, Jonathan Karp, Manuel, Olivier Parcollet, Markus Richter, Hugo U. R. Strand, Nils Wentzell

r"""Generic Green's function container (:class:`~triqs.gfs.gf.Gf`).

This module defines :class:`~triqs.gfs.gf.Gf`, the canonical container for any
TRIQS quantity indexed by a mesh and a target space (Green's
functions, self-energies, hybridization functions, ...). The class
inherits a set of C++-backed methods from :mod:`triqs.gfs.gf_fnt` via
the :class:`~triqs.gfs.gf.AddMethod` metaclass; pure-Python methods (arithmetic,
slicing, lazy initialization) are defined directly here.
"""

import itertools, warnings, numbers
from functools import reduce  # Valid in Python 2.6+, required in Python 3
import operator
import numpy as np
from triqs.mesh import mesh_product
from . import lazy_expressions
from . import descriptors, descriptor_base
from triqs.mesh.mesh_product import MeshProduct
from triqs.plot.protocol import clip_array
from triqs import mesh as meshes
from . import plot
from . import gf_fnt, wrapped_aux
from triqs.mesh.mesh_point import MeshPoint
from triqs.mesh.matsubara_freq import MatsubaraFreq
from operator import mul

# list of all the meshes
all_meshes = (MeshProduct,) + tuple(c for c in list(meshes.__dict__.values()) if isinstance(c, type) and c.__name__.startswith('Mesh'))
# list of call_proxies
all_call_proxies = dict(
    (c.__name__, c) for c in list(wrapped_aux.__dict__.values()) if isinstance(c, type) and c.__name__.startswith('CallProxy')
)


class CallProxyNone:
    """Fallback for :meth:`~triqs.gfs.gf.Gf.__call__` when no compiled 
    C++ proxy fits the Gf.

    Parameters
    ----------
    *args
        Unused.
    """

    def __init__(self, *args):
        pass

    def __call__(self, *args):
        """Always raises :exc:`NotImplementedError`.

        Parameters
        ----------
        *args
            Unused.

        Raises
        ------
        NotImplementedError
            No C++ call proxy was found for this Gf's (mesh, target_rank,
            dtype) combination.
        """
        raise NotImplementedError


# For IO later
def call_factory_from_dict(cl, name, dic):
    """Factory used by HDF5 IO.

    Forwards to ``cl.__factory_from_dict__(name, dic)``; this indirection
    keeps the registration record picklable.

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


# a metaclass that adds all functions of gf_fnt as methods
# the C++ will take care of the dispatch
def add_method_helper(a, cls):
    """Wrap a free function ``a(self, ...)`` as an unbound method.

    Returns a small closure that calls ``a(self, *args, **kw)`` and
    inherits the docstring and ``__name__`` of ``a``, so it shows up in
    ``help(Gf.<name>)`` as the original C++-backed function.

    Parameters
    ----------
    a : callable
        Free function to wrap. Its first positional argument is the
        receiving :class:`~triqs.gfs.gf.Gf` instance.
    cls : type
        Currently unused; kept for signature symmetry with the metaclass
        call site.

    Returns
    -------
    callable
        A function of signature ``(self, *args, **kw)`` suitable for
        ``setattr(cls, name, ...)``.
    """
    def _(self, *args, **kw):
        return a(self, *args, **kw)

    _.__doc__ = a.__doc__
    # _.__doc__ = 50*'-' + '\n' + a.__doc__
    _.__name__ = a.__name__
    return _


class AddMethod(type):
    """Metaclass that injects every callable in :mod:`triqs.gfs.gf_fnt` as 
    a :class:`~triqs.gfs.gf.Gf` method.

    At class-creation time, iterates over every callable in
    :mod:`triqs.gfs.gf_fnt` and, for names not already defined on the
    new class, sets the attribute to the wrapper produced by
    :func:`~triqs.gfs.gf.add_method_helper`. Dispatch by mesh and target type 
    is handled in C++.

    Parameters
    ----------
    cls : type
        The class being created (i.e. :class:`~triqs.gfs.gf.Gf` or a subclass).
    name : str
        Name of the class being created.
    bases : tuple of type
        Base classes of ``cls``.
    dct : dict
        Class body namespace.
    """

    def __init__(cls, name, bases, dct):
        super(AddMethod, cls).__init__(name, bases, dct)
        for a in [f for f in list(gf_fnt.__dict__.values()) if callable(f)]:
            if not hasattr(cls, a.__name__):
                setattr(cls, a.__name__, add_method_helper(a, cls))


class Idx:
    r"""Tag wrapping a mesh-point linear index for use in :class:`~triqs.gfs.gf.Gf` 
    ``[]`` access.

    ``Idx(i)`` (or ``Idx(i, j, ...)`` for a :class:`~triqs.mesh.mesh_product.MeshProduct`) 
    marks its argument(s) as **linear indices** along the mesh axis rather than
    target-space indices, e.g. ``g[Idx(0)]`` returns the value of ``g``
    at the first mesh point.

    Parameters
    ----------
    *x : int
        One index per mesh axis. A single argument is stored as a scalar;
        multiple arguments are stored as a tuple.

    Attributes
    ----------
    index : int or tuple of int
        The wrapped index (scalar for a single mesh, tuple for a
        :class:`~triqs.mesh.mesh_product.MeshProduct`).

    See Also
    --------
    triqs.mesh.mesh_point.MeshPoint : Mesh-point object that can also be used in
        ``Gf`` ``[]`` access.
    """

    def __init__(self, *x):
        self.index = x[0] if len(x) == 1 else x


class Gf(metaclass=AddMethod):
    r"""Container for Green's functions and related quantities.

    A :class:`~triqs.gfs.gf.Gf` is a container for generic functions defined on
    meshes:

    .. math::

      G : \mathcal{M} \to T .

    Here, :math:`\mathcal{M}` is the domain of the function, determined 
    entirely by the underlying mesh (see :mod:`triqs.mesh.meshes`) or 
    :class:`~triqs.mesh.mesh_product.MeshProduct`, and :math:`T` is the 
    target space, which determines what quantities are stored at each
    mesh point (real/complex scalars, matrices, tensors).

    Typical use cases include

    - scalar-valued imaginary time Green's functions

      .. math::

        G(\tau) \equiv - \mathcal{T} \langle c(\tau) c^{\dagger} (0)\rangle
        \qquad \text{ for } 0 \leq \tau \leq \beta \; ,

      with :class:`~triqs.mesh.meshes.MeshImTime` as the underlying mesh
      and :math:`T = \mathbb{R}` as the target space.
    - matrix-valued Matsubara Green's functions

      .. math::

        G_{\alpha \beta} (i \omega_n) \equiv \int_0^\beta 
        G_{\alpha \beta}(\tau) e^{i \omega_n \tau} d\tau \; ,

      with :class:`~triqs.mesh.meshes.MeshImFreq` as the underlying mesh
      and :math:`T = \mathbb{C}^{N \times N}` as the target space.

    Under the hood, :class:`~triqs.gfs.gf.Gf` stores a contiguous ``numpy.ndarray`` of
    shape ``(*mesh_sizes, *target_shape)`` (see :attr:`~triqs.gfs.gf.Gf.data`). What is
    stored and how Green's functions are evaluated depends on the mesh.

    Supported features include:

    - Bracket lookup ``g[...]`` and call-style evaluation ``g(...)``
      (see Notes).
    - Element-wise arithmetic (``+``, ``-``, ``*``, ``/``, ``@``) and
      in-place variants. Scalars broadcast along the target-space
      diagonal for square matrix targets; mixing two :class:`~triqs.gfs.gf.Gf`
      requires identical meshes.
    - Lazy initialization via ``g << expr`` from descriptors
      (e.g. ``g << iOmega_n + 0.5``, ``g << SemiCircular(1.0)``).
      See :meth:`~triqs.gfs.gf.Gf.__lshift__` and :mod:`triqs.gfs.descriptors`.
    - HDF5 read/write through :class:`h5.HDFArchive`.
    - Target-space slicing and :attr:`~triqs.gfs.gf.Gf.real` / :attr:`~triqs.gfs.gf.Gf.imag` views.

    Parameters
    ----------
    mesh : Mesh or MeshProduct
        Mesh on which the Green's function is defined.
    data : numpy.ndarray, optional
        Raw storage of shape ``(*mesh_sizes, *target_shape)``. Mutually
        exclusive with :attr:`~triqs.gfs.gf.Gf.target_shape`.
    target_shape : list of int, optional
        Shape of the target space (e.g. ``[2, 2]`` for a 2x2 matrix
        Green's function, ``[]`` for a scalar). Mutually exclusive with
        :attr:`~triqs.gfs.gf.Gf.data`.
    is_real : bool, optional
        If ``True`` (and :attr:`~triqs.gfs.gf.Gf.target_shape` is given), allocate the data
        as ``float64`` instead of ``complex128``. Has no effect when
        :attr:`~triqs.gfs.gf.Gf.data` is supplied. Default ``False``.
    name : str, optional
        Name used for plot labels. Default ``''``.
    indices : list, optional
        Deprecated. String indices are no longer supported; passing this
        argument emits a :class:`FutureWarning` and the lengths are used
        to derive :attr:`~triqs.gfs.gf.Gf.target_shape`.

    Attributes
    ----------
    mesh : Mesh
        The mesh of the Green's function.
    data : numpy.ndarray
        Raw data, shape ``(*mesh_sizes, *target_shape)``.
    rank : int
        Number of mesh axes (``mesh.rank``).
    target_rank : int
        Number of target-space axes (``len(target_shape)``).
    target_shape : tuple of int
        Shape of the target space.
    name : str
        Plot label.
    real : Gf
        Views of the real part of the :attr:`~triqs.gfs.gf.Gf.data`.
    imag : Gf
        Views of imaginary real part of the :attr:`~triqs.gfs.gf.Gf.data`.

    Notes
    -----
    There are subtle differences when accessing a :class:`~triqs.gfs.gf.Gf` with brackets
    ``[]`` vs. parentheses ``()``:

    - ``g[x]`` looks up the value at an *existing* mesh point, with ``x``
      an :class:`~triqs.gfs.gf.Idx`, :class:`~triqs.mesh.mesh_point.MeshPoint`, or
      :class:`~triqs.mesh.matsubara_freq.MatsubaraFreq`.
    - ``g(x)`` instead *evaluates* the Green's function at an arbitrary ``x``
      using the interpolation rule the mesh declares -- linear in imaginary time,
      exact in Matsubara, k-linear on the Brillouin zone, basis expansion on
      DLR / Legendre, etc. The mesh, not the :class:`~triqs.gfs.gf.Gf`, decides what
      "evaluation" means.

    Examples
    --------
    Construct a scalar imaginary-time Green's function and a 2x2
    matrix-valued Matsubara Green's function:

    >>> from triqs.gfs import Gf
    >>> from triqs.mesh import MeshImTime, MeshImFreq
    >>> tau_mesh = MeshImTime(beta=10.0, statistic='Fermion', n_tau=2049)
    >>> g_tau    = Gf(mesh=tau_mesh, target_shape=[])
    >>> iw_mesh  = MeshImFreq(beta=10.0, statistic='Fermion', n_iw=1024)
    >>> g_iw     = Gf(mesh=iw_mesh, target_shape=[2, 2])

    Initialize lazily from a descriptor expression:

    >>> from triqs.gfs import iOmega_n, SemiCircular
    >>> g_iw  << iOmega_n + 0.5
    >>> g_tau << SemiCircular(half_bandwidth=1.0)

    Access the raw storage as a numpy array:

    >>> g_iw.data.shape
    (2048, 2, 2)
    >>> g_iw.data[:] = 0.0                  # zero in place
    """

    _hdf5_data_scheme_ = 'Gf'
    __array_priority__ = 10000  # Makes sure the operations of this class are applied as priority

    def __init__(self, **kw):  # enforce keyword only policy
        # print "Gf construct args", kw

        def delegate(self, mesh, data=None, target_shape=None, name='', is_real=False, indices=None):
            """
            target_shape and data  : must provide exactly one of them
            """
            # FIXME ? name is deprecated
            # if name:
            #    warnings.warn("constructor parameter 'name' is deprecated in gf constructor.\n It is only used in plots.\n Pass the name to the oplot function directly")
            self.name = name

            # input check
            assert (target_shape is None) or (data is None), 'data and target_shape : one must be None'
            assert (data is None) or (is_real is False), 'is_real can not be True if data is not None'
            if target_shape:
                for i in target_shape:
                    assert i > 0, 'Target shape elements must be >0'

            # mesh
            assert isinstance(mesh, all_meshes), 'Mesh is unknown. Possible type of meshes are %s' % ', '.join(
                [m.__name__ for m in all_meshes]
            )
            self._mesh = mesh

            # indices backward compat layer
            if indices is not None:
                warnings.warn('The use of string indices is no longer supported, converting to target_shape instead.', FutureWarning)
                assert target_shape is None, 'target_shape must be None if indices is not None'
                if isinstance(indices[0], (list, range)):
                    target_shape = [len(li) for li in indices]
                else:
                    target_shape = [len(indices), len(indices)]

            # data
            if data is None:
                # if no data, we need target_shape
                assert target_shape is not None, 'target_shape must be provided if data is None'
                # we now allocate the data
                l = mesh.size_of_components() if isinstance(mesh, MeshProduct) else [len(mesh)]
                data = np.zeros(list(l) + list(target_shape), dtype=np.float64 if is_real else np.complex128)
            else:
                l = tuple(mesh.size_of_components()) if isinstance(mesh, MeshProduct) else (len(mesh),)
                assert l == data.shape[0 : len(l)], 'Mismatch between data shape %s and sizes of mesh(es) %s\n ' % (data.shape, l)

            # Now we have the data at correct size. Set up a few short cuts
            self._data = data
            len_data_shape = len(self._data.shape)
            self._target_rank = len_data_shape - (self._mesh.rank if isinstance(mesh, MeshProduct) else 1)
            self._rank = len_data_shape - self._target_rank
            assert self._rank >= 0

            # target_shape. Ensure it is correct in any case.
            assert target_shape is None or tuple(target_shape) == self._data.shape[self._rank :]  # Debug only
            self._target_shape = self._data.shape[self._rank :]

            # NB : at this stage, enough checks should have been made in Python in order for the C++ view
            # to be constructed without any INTERNAL exceptions.
            # Set up the C proxy for call operator for speed. The name has to
            # agree with the wrapped_aux module, it is of only internal use
            s = (
                '_x_'.join(m.__class__.__name__[4:] for m in self.mesh._mlist)
                if isinstance(mesh, MeshProduct)
                else self._mesh.__class__.__name__[4:]
            )
            proxyname = 'CallProxy%s_%s%s' % (s, self.target_rank, '_R' if data.dtype == np.float64 else '')
            try:
                self._c_proxy = all_call_proxies.get(proxyname, CallProxyNone)(self)
            except:
                self._c_proxy = None

            # check all invariants. Debug.
            self.__check_invariants()

        delegate(self, **kw)

    def __check_invariants(self):
        """Check various invariants. Mainly for debug."""
        # rank
        assert self.rank == self._mesh.rank if isinstance(self._mesh, MeshProduct) else 1
        # The mesh size must correspond to the size of the data
        assert (
            self._data.shape[: self._rank] == tuple(len(m) for m in self._mesh.components)
            if isinstance(self._mesh, MeshProduct)
            else (len(self._mesh),)
        )

    def density(self, *args, **kwargs):
        r"""Compute the single-particle density matrix.

        Equivalent to :math:`\langle c^\dagger_i c_j \rangle` evaluated
        from the diagonal-frequency / equal-time limit of the Green's
        function.

        Parameters
        ----------
        beta : float, optional
            Inverse temperature. Required only for finite-temperature
            density evaluation on a :class:`~triqs.mesh.meshes.MeshReFreq` mesh.

        Returns
        -------
        density_matrix : numpy.ndarray
            Single-particle density matrix of shape ``target_shape``.

        Notes
        -----
        Only available for single-mesh Green's functions on a Matsubara,
        real-frequency or Legendre mesh.
        """

        return gf_fnt.density(self, *args, **kwargs)

    @property
    def rank(self):
        r"""Mesh rank — number of mesh axes.

        Returns
        -------
        int
            ``1`` for a single mesh, otherwise ``len(mesh.components)``
            for a :class:`~triqs.mesh.mesh_product.MeshProduct`.
        """
        return self._rank

    @property
    def target_rank(self):
        """Number of target-space axes.

        Returns
        -------
        int
            Equal to ``len(target_shape)`` — ``0`` for a scalar target,
            ``2`` for a matrix target, etc.
        """
        return self._target_rank

    @property
    def target_shape(self):
        """Shape of the target space.

        Returns
        -------
        tuple of int
            Target-space dimensions, e.g. ``(2, 2)`` for a 2x2 matrix
            target or ``()`` for a scalar target.
        """
        return self._target_shape

    @property
    def target_indices(self):
        """Iterate over every target-space index tuple in C order.

        Returns
        -------
        iterator of tuple of int
            ``itertools.product`` over ``range(d)`` for each ``d`` in
            :attr:`~triqs.gfs.gf.Gf.target_shape`.
        """
        return itertools.product(*[range(i) for i in self._target_shape])

    @property
    def indices(self):
        """Deprecated. Use :attr:`~triqs.gfs.gf.Gf.target_shape` / 
        :attr:`~triqs.gfs.gf.Gf.target_indices` instead.

        Returns
        -------
        list of range
            ``[range(d) for d in target_shape]``.

        Warns
        -----
        FutureWarning
            Emitted on every access; this property is deprecated.
        """
        warnings.warn('Gf.indices is deprecated, use Gf.target_shape', FutureWarning)
        return [range(d) for d in self.target_shape]

    @property
    def mesh(self):
        """The mesh of the Green's function.

        Returns
        -------
        Mesh or MeshProduct
            The underlying mesh — a single :mod:`triqs.mesh` instance,
            or a :class:`~triqs.mesh.mesh_product.MeshProduct` for
            multi-variable Green's functions.
        """
        return self._mesh

    @property
    def data(self):
        r"""Raw storage of the Green's function.

        The storage convention is

        .. math::

            \texttt{data}[x_1, \ldots, x_r,\, n_1, \ldots, n_t]
            \;=\; G(x_1, \ldots, x_r)_{n_1, \ldots, n_t},

        where :math:`x_i` index the mesh and :math:`n_j` index the
        target space.

        Returns
        -------
        numpy.ndarray
            Contiguous array of shape ``(*mesh_sizes, *target_shape)``,
            dtype ``complex128`` by default or ``float64`` if the Gf
            was constructed with ``is_real=True``.
        """
        return self._data

    def copy(self):
        """Return an independent deep copy of ``self``.

        Returns
        -------
        Gf
            Copy with freshly allocated mesh and data arrays.
        """
        return Gf(mesh=self._mesh.copy(), data=self._data.copy(), name=self.name)

    def copy_from(self, another):
        """In-place copy from ``another`` into ``self``.

        Parameters
        ----------
        another : Gf
            Source Green's function. Must have an identical data shape;
            its mesh is copied into ``self.mesh``.

        Raises
        ------
        AssertionError
            If ``self.data.shape != another.data.shape``.
        """
        self._mesh.copy_from(another.mesh)
        assert self._data.shape == another._data.shape, (
            'Shapes are incompatible: ' + str(self._data.shape) + ' vs ' + str(another._data.shape)
        )
        self._data[:] = another._data[:]
        self.__check_invariants()

    def __repr__(self):
        """One-line summary of the Green's function.

        Returns
        -------
        str
            ``"Green's Function <name> with mesh <mesh> and target_shape <target_shape>"``.
        """
        return "Green's Function %s with mesh %s and target_shape %s: \n" % (self.name, self.mesh, self.target_shape)

    def __str__(self):
        """Alias for :meth:`~triqs.gfs.gf.Gf.__repr__`.

        Returns
        -------
        str
        """
        return self.__repr__()

    # --------------  Bracket operator []  -------------------------

    # Helper: Convert Idx or MeshPoint into data_index, else forward argument
    @staticmethod
    def _to_data_index(x, m):
        if isinstance(x, MeshPoint):
            assert x.mesh_hash == m.mesh_hash, 'Green function Mesh and MeshPoint have incompatible hash'
            return x.data_index
        elif isinstance(x, Idx):
            return m.to_data_index(x.index)
        elif isinstance(x, MatsubaraFreq):
            return m.to_data_index(x)
        else:
            return x

    _full_slice = slice(None, None, None)

    def __getitem__(self, key):
        r"""Return a mesh point, a sliced view, or a target-space sub-block.

        The bracket operator dispatches on the type of ``key``:

        * ``g[:]`` — return ``self`` (so that ``g[:] << RHS`` is
          equivalent to ``g << RHS``).
        * ``g[mp]`` with ``mp`` a :class:`~triqs.mesh.mesh_point.MeshPoint`,
          :class:`~triqs.gfs.gf.Idx` or :class:`~triqs.mesh.matsubara_freq.MatsubaraFreq` — 
          return the target-space slab at that mesh point (a numpy array).
        * ``g[mp0, mp1, ...]`` — same, for a :class:`~triqs.mesh.mesh_product.MeshProduct`.
        * ``g[mp, :]`` (mix of mesh points and ``:``) — return a
          :class:`~triqs.gfs.gf.Gf` on the remaining mesh axes.
        * ``g[i, j, ...]`` with all-integer indices — extract a
          ``target_rank``-dimensional sub-block as a :class:`~triqs.gfs.gf.Gf` view of
          reduced target rank.
        * ``g[i:j, ...]`` with all-slice indices — slice the target
          space and return a :class:`~triqs.gfs.gf.Gf` view.

        Parameters
        ----------
        key : slice, MeshPoint, Idx, MatsubaraFreq, int, or tuple thereof
            See the bullet list above.

        Returns
        -------
        Gf or numpy.ndarray
            A :class:`~triqs.gfs.gf.Gf` view when the operation slices the mesh or
            target space; a numpy array when ``key`` resolves to a
            single mesh point.

        Notes
        -----
        Partial slicing of the mesh and string indices are not
        supported.
        """
        # First case : g[:] = RHS ... will be g << RHS
        if key == self._full_slice:
            return self

            # Only one argument. Must be either one of (MeshPoint, Idx, MatsubaraFreq) or slicing rank1 target space
        if not isinstance(key, tuple):
            if isinstance(key, (Idx, MeshPoint, MatsubaraFreq)):
                return self.data[self._to_data_index(key, self._mesh)]
            else:
                key = (key,)

        # If all arguments are MeshPoint, we are slicing the mesh or evaluating
        if all(isinstance(x, (MeshPoint, Idx, MatsubaraFreq)) for x in key):
            assert len(key) == self.rank, 'wrong number of arguments in [ ]. Expected %s, got %s' % (self.rank, len(key))
            return self.data[tuple(self._to_data_index(x, m) for x, m in zip(key, self._mesh._mlist))]

        # If any argument is a MeshPoint, we are slicing the mesh or evaluating
        elif any(isinstance(x, (MeshPoint, Idx, MatsubaraFreq)) for x in key):
            assert len(key) == self.rank, 'wrong number of arguments in [[ ]]. Expected %s, got %s' % (self.rank, len(key))
            assert all(
                isinstance(x, (MeshPoint, Idx, MatsubaraFreq, slice)) for x in key
            ), "Invalid accessor of Green's function, please combine only MeshPoints, Idx and slice"
            assert self.rank > 1, 'Internal error : impossible case'  # here all == any for one argument
            mlist = self._mesh._mlist
            for x in key:
                if isinstance(x, slice) and x != self._full_slice:
                    raise NotImplementedError('Partial slice of the mesh not implemented')
            # slice the data
            k = tuple(self._to_data_index(x, m) for x, m in zip(key, mlist)) + self._target_rank * (slice(0, None),)
            dat = self._data[k]
            # list of the remaining lists
            mlist = [m for i, m in filter(lambda tup_im: not isinstance(tup_im[0], (MeshPoint, Idx, MatsubaraFreq)), zip(key, mlist))]
            assert len(mlist) > 0, 'Internal error'
            mesh = MeshProduct(*mlist) if len(mlist) > 1 else mlist[0]
            sing = None
            r = Gf(mesh=mesh, data=dat)
            r.__check_invariants()
            return r

        # In all other cases, we are slicing the target space
        else:
            assert self.target_rank == len(key), 'wrong number of arguments. Expected %s, got %s' % (self.target_rank, len(key))

            # Slicing with ranges
            if all(isinstance(x, slice) for x in key):
                key_tpl = tuple(key)

            # Integer access
            elif all(isinstance(x, int) for x in key):
                key_tpl = tuple(key)

            # Invalid Access
            elif any(isinstance(x, str) for x in key):
                raise RuntimeError('String indices are no longer supported. Please use integer indices for the target.')
            else:
                raise NotImplementedError('Partial slice of the target space not implemented')

            dat = self._data[self._rank * (slice(0, None),) + key_tpl]
            r = Gf(mesh=self._mesh, data=dat)

            r.__check_invariants()
            return r

    def __setitem__(self, key, val):
        """Assign at a mesh point or delegate to ``<<`` for everything else.

        ``g[mp] = val`` writes ``val`` into ``self.data`` at the data
        index corresponding to ``mp`` (a :class:`~triqs.mesh.mesh_point.MeshPoint`, 
        :class:`~triqs.gfs.gf.Idx` or :class:`~triqs.mesh.matsubara_freq.MatsubaraFreq`, 
        or a tuple of those for a :class:`~triqs.mesh.mesh_product.MeshProduct`). Any 
        other shape of ``key`` falls back to ``self[key] << val``.

        Parameters
        ----------
        key : MeshPoint, Idx, MatsubaraFreq, tuple thereof 
            Assignment target.
        val : array-like or Gf-compatible
            Value or Green's function to assign.
        """
        # Only one argument and not a slice. Must be a mesh point, Idx
        if isinstance(key, (MeshPoint, Idx, MatsubaraFreq)):
            self.data[self._to_data_index(key, self._mesh)] = val

        # If all arguments are MeshPoint, we are slicing the mesh or evaluating
        elif isinstance(key, tuple) and all(isinstance(x, (MeshPoint, Idx, MatsubaraFreq)) for x in key):
            assert len(key) == self.rank, 'wrong number of arguments in [ ]. Expected %s, got %s' % (self.rank, len(key))
            self.data[tuple(self._to_data_index(x, m) for x, m in zip(key, self._mesh._mlist))] = val

        else:
            self[key] << val

    # -------------- Various operations -------------------------------------

    @property
    def real(self):
        """Real-part view sharing the underlying mesh.

        Returns
        -------
        Gf
            A new :class:`~triqs.gfs.gf.Gf` on the same mesh, backed by
            ``self.data.real`` (a view, not a copy). Its ``name`` is
            prefixed with ``'Re '`` when ``self.name`` is non-empty.
        """
        return Gf(mesh=self._mesh, data=self._data.real, name=('Re ' + self.name) if self.name else '')

    @property
    def imag(self):
        """Imaginary-part view sharing the underlying mesh.

        Returns
        -------
        Gf
            A new :class:`~triqs.gfs.gf.Gf` on the same mesh, backed by
            ``self.data.imag`` (a view, not a copy). Its ``name`` is
            prefixed with ``'Im '`` when ``self.name`` is non-empty.
        """
        return Gf(mesh=self._mesh, data=self._data.imag, name=('Im ' + self.name) if self.name else '')

    # --------------  Lazy system -------------------------------------

    def __lazy_expr_eval_context__(self):
        return LazyCTX(self)

    def __lshift__(self, A):
        r"""Lazy initialization / copy operator (``g << RHS``).

        Parameters
        ----------
        A : Gf, descriptor or :class:`~triqs.gfs.lazy_expressions.LazyExpr`
            * If ``A`` is a :class:`~triqs.gfs.gf.Gf` on the same mesh, copy it into
              ``self``.
            * If ``A`` is a descriptor (e.g. :class:`~triqs.gfs.descriptors.SemiCircular`,
              :class:`~triqs.gfs.descriptors.Flat`, :data:`~triqs.gfs.descriptor_base.iOmega_n`) 
              or a lazy expression built from descriptors and scalars, evaluate it on
              ``self.mesh`` and store the result.
            * If ``A`` is a scalar, fill the diagonal of the target
              space with that scalar.

        Returns
        -------
        Gf
            ``self`` (so that ``g << RHS`` can be chained).

        Examples
        --------
        >>> g << iOmega_n + 0.5
        >>> g << SemiCircular(half_bandwidth=1.0)
        >>> g2 << g
        """
        if isinstance(A, Gf):
            if self is not A:  # otherwise it is useless AND does not work !!
                assert self.mesh == A.mesh, 'Green function meshes are not compatible:\n  %s\nand\n  %s' % (self.mesh, A.mesh)
                self.copy_from(A)
        elif isinstance(A, lazy_expressions.LazyExpr):  # A is a lazy_expression made of GF, scalars, descriptors
            A2 = descriptors.convert_scalar_to_const(A)

            def e_t(x):
                if not isinstance(x, descriptors.Base):
                    return x
                tmp = self.copy()
                x(tmp)
                return tmp

            self.copy_from(lazy_expressions.eval_expr_with_context(e_t, A2))
        elif isinstance(A, lazy_expressions.LazyExprTerminal):  # e.g. g<< SemiCircular (...)
            self << lazy_expressions.LazyExpr(A)
        elif descriptors.is_scalar(A):  # in the case it is a scalar ....
            self << lazy_expressions.LazyExpr(A)
        else:
            raise NotImplemented
        return self

    # -------------- call -------------------------------------

    def __call__(self, *args):
        r"""Evaluate the Green's function at the given point(s).

        Parameters
        ----------
        *args
            One value per mesh axis. Each value may be a raw scalar
            (e.g. a complex frequency, a real frequency, an imaginary
            time, ...), a :class:`~triqs.mesh.mesh_point.MeshPoint` or an 
            :class:`~triqs.gfs.gf.Idx`.

        Returns
        -------
        numpy.ndarray
            Value of the Green's function at the requested point, of
            shape ``target_shape``. Interpolation between mesh points is
            applied where supported by the C++ backend.

        Notes
        -----
        Dispatches to a C++ ``CallProxy*`` selected from the mesh and
        target rank. For mesh / target combinations without a proxy this
        method raises :class:`NotImplementedError`.
        """
        assert self._c_proxy, ' no proxy'

        def filt(x):
            if isinstance(x, MeshPoint):
                if x.value is not None:
                    return x.value
                return x.index
            elif isinstance(x, Idx):
                return x.index
            return x

        return self._c_proxy(*[filt(x) for x in args])

    # -------------- Various operations -------------------------------------

    def __le__(self, other):
        raise RuntimeError(' Operator <= not defined ')

    # ---------- Addition

    def __iadd__(self, arg):
        """In-place addition ``self += arg`` (element-wise on ``self.data``).

        Parameters
        ----------
        arg : Gf, descriptor, lazy expression, scalar or numpy.ndarray
            Operand. A scalar added to a matrix-valued Gf is broadcast
            along the diagonal of the target space.

        Returns
        -------
        Gf or LazyExpr
            ``self`` (or a lazy expression if ``arg`` is lazy).
        """
        if descriptor_base.is_lazy(arg):
            return lazy_expressions.make_lazy(self) + arg
        if isinstance(arg, Gf):
            assert type(self.mesh) == type(arg.mesh), 'Can not add two Gf with meshes of different type'
            assert self.mesh == arg.mesh, 'Can not add two Gf with different mesh'
            self._data += arg._data
        else:
            if self._target_rank == 2:
                if not isinstance(arg, np.ndarray):
                    assert self._data.shape[-1] == self._data.shape[-2], 'Adding a scalar to a Gf with non-square target_shape is not supported'
                    n = self._data.shape[-1]
                    arg = arg * np.eye(n, dtype=self._data.dtype)
                self._data[:] += arg
            elif not isinstance(arg, np.ndarray):
                self._data[:] += arg
            else:
                raise NotImplemented
        return self

    def __add__(self, y):
        """Element-wise addition; returns a new :class:`~triqs.gfs.gf.Gf`.

        Parameters
        ----------
        y : Gf, descriptor, lazy expression, scalar or numpy.ndarray
            Right-hand operand.

        Returns
        -------
        Gf or LazyExpr
            ``self + y`` as a fresh object.
        """
        c = self.copy()
        c += y
        return c

    def __radd__(self, y):
        """Reflected addition ``y + self``; commutes with :meth:`~triqs.gfs.gf.Gf.__add__`.

        Parameters
        ----------
        y : same as :meth:`~triqs.gfs.gf.Gf.__add__`

        Returns
        -------
        Gf or LazyExpr
        """
        return self.__add__(y)

    # ---------- Substraction

    def __isub__(self, arg):
        """In-place subtraction ``self -= arg``.

        Parameters
        ----------
        arg : Gf, descriptor, lazy expression, scalar or numpy.ndarray
            Operand.

        Returns
        -------
        Gf or LazyExpr
        """
        if descriptor_base.is_lazy(arg):
            return lazy_expressions.make_lazy(self) - arg
        if isinstance(arg, Gf):
            assert type(self.mesh) == type(arg.mesh), 'Can not subtract two Gf with meshes of different type'
            assert self.mesh == arg.mesh, 'Can not subtract two Gf with different mesh'
            self._data -= arg._data
        else:
            if self._target_rank == 2:
                if not isinstance(arg, np.ndarray):
                    assert self._data.shape[-1] == self._data.shape[-2], 'Subtracting a scalar from a Gf with non-square target_shape is not supported'
                    n = self._data.shape[-1]
                    arg = arg * np.eye(n, dtype=self._data.dtype)
                self._data[:] -= arg
            elif not isinstance(arg, np.ndarray):
                self._data[:] -= arg
            else:
                raise NotImplemented
        return self

    def __sub__(self, y):
        """Element-wise subtraction; returns a new :class:`~triqs.gfs.gf.Gf`.

        Parameters
        ----------
        y : same as :meth:`~triqs.gfs.gf.Gf.__isub__`

        Returns
        -------
        Gf or LazyExpr
            ``self - y``.
        """
        c = self.copy()
        c -= y
        return c

    def __rsub__(self, y):
        """Reflected subtraction ``y - self``.

        Parameters
        ----------
        y : same as :meth:`~triqs.gfs.gf.Gf.__sub__`

        Returns
        -------
        Gf or LazyExpr
        """
        c = (-1) * self.copy()
        c += y
        return c

    # ---------- Matrix Multiplication (@)

    def __imatmul__(self, arg):
        """In-place matrix multiplication ``self @= arg`` on the target space.

        Evaluated pointwise on the mesh. Rank-0 (scalar-valued) and
        rank-2 (matrix-valued) Green's functions are supported; mixed
        ``rank-0 * rank-2`` broadcasts the scalar value across the
        target-space matrix.

        Parameters
        ----------
        arg : Gf, lazy expression, scalar or numpy.ndarray
            Right-hand operand.

        Returns
        -------
        Gf or LazyExpr
        """
        if descriptor_base.is_lazy(arg):
            return lazy_expressions.make_lazy(self) * arg
        # If arg is a Gf
        if isinstance(arg, Gf):
            assert type(self.mesh) == type(arg.mesh), 'Can not multiply two Gf with meshes of different type'
            assert self.mesh == arg.mesh, 'Can not use in-place multiplication for two Gf with different mesh'

            if self.target_rank == 2:
                if arg.target_rank == 2:
                    assert arg.target_shape[0] != arg.target_shape[1], 'In place multiplication not supported if'
                    'the argument is a nonsquare matrix. Use regular multiplication instead.'
                    np.matmul(self.data, arg.data, out=self.data)
                elif arg.target_rank == 0:
                    self.data[:] *= arg.data[..., None, None]
                else:
                    raise NotImplementedError('argument of in place multiplication must be rank 0 or 2')

            elif self.target_rank == 0:
                assert arg.target_rank == 0, 'argument of in place multiplication must have rank 0 if self does'
                self.data[:] = self.data * arg.data

            else:
                raise NotImplementedError("Green's functions must be of rank 0 or 2 for multiplication")

        elif isinstance(arg, numbers.Number):
            self._data[:] *= arg
        elif isinstance(arg, np.ndarray):
            assert len(arg.shape) == 2, 'Multiplication only supported for matrices'
            assert len(self.target_shape) == 2, 'Multiplication only supported for matrix_valued Gfs'
            self.data[:] = np.tensordot(self.data, arg, axes=([-1], [-2]))
        else:
            assert False, 'Invalid operand type for Gf in-place multiplication'
        return self

    @staticmethod
    def _combine_mesh_mul(l, r):
        """Apply the Fermion/Boson rules for ImTime mesh, and recursively for MeshProduct."""
        assert type(l) == type(r), 'Can not multiply two Gf with meshes of different type'

        if type(l) is MeshProduct:
            return MeshProduct(*[Gf._combine_mesh_mul(l, r) for (l, r) in zip(l.components, r.components)])

        if not type(l) is meshes.MeshImTime:  # regular case
            assert l == r, 'Can not multiply two Gf with different mesh'
            return l.copy()
        else:
            assert abs(l.beta - r.beta) < 1.0e-15 and len(l) == len(r), 'Can not multiply two Gf with different mesh'
            return meshes.MeshImTime(l.beta, 'Boson' if l.statistic == r.statistic else 'Fermion', len(l))

    def __matmul__(self, y):
        """Matrix multiplication ``self @ y`` on the target space.

        For Matsubara meshes the result mesh statistic follows the
        bosonic / fermionic combination rule of the operands.

        Parameters
        ----------
        y : Gf, scalar or numpy.ndarray
            Right-hand operand.

        Returns
        -------
        Gf
            Product evaluated pointwise on the mesh; freshly allocated.
        """
        if isinstance(y, Gf):
            # make a copy, but special treatment of the mesh in the Imtime case.
            result_mesh = Gf._combine_mesh_mul(self._mesh, y.mesh)
            if self.target_rank == 2 and y.target_rank == 2:
                c = Gf(mesh=result_mesh, target_shape=[self.target_shape[0], y.target_shape[1]])
                np.matmul(self.data, y.data, out=c.data)
            elif self.target_rank == 0 and y.target_rank == 0:
                c = Gf(mesh=result_mesh, target_shape=[])
                c.data[:] = self.data * y.data
            elif self.target_rank == 2 and y.target_rank == 0:
                c = Gf(mesh=result_mesh, target_shape=self.target_shape)
                c.data[:] = self.data * y.data[..., None, None]
            elif self.target_rank == 0 and y.target_rank == 2:
                c = Gf(mesh=result_mesh, target_shape=y.target_shape)
                c.data[:] = self.data[..., None, None] * y.data

            else:
                raise NotImplementedError("Green's functions must be of rank 0 or 2 for multiplication")

        elif isinstance(y, (numbers.Number, np.ndarray)):
            c = self.copy()
            c *= y
        else:
            assert False, 'Invalid operand type for Gf multiplication'
        return c

    def __rmatmul__(self, y):
        """Reflected matrix multiplication ``y @ self``.

        Parameters
        ----------
        y : scalar or 2D numpy.ndarray
            Left-hand operand.

        Returns
        -------
        Gf
        """
        c = self.copy()
        if isinstance(y, np.ndarray):
            assert len(y.shape) == 2, 'Multiplication only supported for matrices'
            assert len(self.target_shape) == 2, 'Multiplication only supported for matrix_valued Gfs'
            c.data[:] = np.moveaxis(np.tensordot(y, self.data, axes=([-1], [-2])), 0, -2)
        elif isinstance(y, numbers.Number):
            c *= y
        else:
            assert False, 'Invalid operand type for Gf multiplication'
        return c

    # ---------- Multiplication (with *, for now it's equivalent to the matrix multiplication)
    def __imul__(self, arg):
        """In-place multiplication ``self *= arg`` (alias for :meth:`~triqs.gfs.gf.Gf.__imatmul__`).

        Parameters
        ----------
        arg : Gf, scalar or numpy.ndarray

        Returns
        -------
        Gf
        """
        return self.__imatmul__(arg)

    def __mul__(self, y):
        """Multiplication ``self * y`` (alias for :meth:`~triqs.gfs.gf.Gf.__matmul__`).

        For matrix-valued Green's functions this is the target-space
        matrix product evaluated pointwise on the mesh.

        Parameters
        ----------
        y : Gf, scalar or numpy.ndarray

        Returns
        -------
        Gf
        """
        return self.__matmul__(y)

    def __rmul__(self, y):
        """Reflected multiplication ``y * self``.

        Parameters
        ----------
        y : scalar or numpy.ndarray

        Returns
        -------
        Gf
        """
        return self.__rmatmul__(y)

    # ---------- Division
    def __itruediv__(self, arg):
        """In-place scalar division ``self /= arg``.

        Parameters
        ----------
        arg : scalar
            Divisor applied element-wise to ``self.data``.

        Returns
        -------
        Gf
        """
        self._data[:] /= arg
        return self

    def __truediv__(self, y):
        """Scalar division ``self / y``; returns a new :class:`~triqs.gfs.gf.Gf`.

        Parameters
        ----------
        y : scalar
            Divisor.

        Returns
        -------
        Gf
        """
        c = self.copy()
        c /= y
        return c

    # ---------- unary -
    def __neg__(self):
        """Unary minus ``-self``; returns a new :class:`~triqs.gfs.gf.Gf` with sign-flipped data.

        Returns
        -------
        Gf
        """
        c = self.copy()
        c *= -1
        return c

    # ----------------------------- other operations -----------------------------------

    def invert(self):
        """Inverts the Green's function (in place)."""

        if self.target_rank == 0:  # Scalar target space
            self.data[:] = 1.0 / self.data
        elif self.target_rank == 2:  # Matrix target space
            self.data[:] = np.linalg.inv(self.data)
        else:
            raise TypeError("Inversion only makes sense for matrix or scalar_valued Green's functions")

    def inverse(self):
        """Computes the inverse of the Green's function.

        Returns
        -------
        G : Gf (copy)
            The matrix/scalar inverse of the Green's function.
        """
        r = self.copy()
        r.invert()
        return r

    def transpose(self):
        """Take the transpose of a matrix valued Green's function.

        Returns
        -------
        G : Gf (copy)
            The transpose of the Green's function.

        Notes
        -----
        Only implemented for single mesh matrix valued Green's functions.

        """

        # FIXME Why this assert ?
        # assert any( (isinstance(self.mesh, x) for x in [meshes.MeshImFreq, meshes.MeshReFreq])), "Method invalid for this Gf"

        assert self.rank == 1, "Transpose only implemented for single mesh Green's functions"
        assert self.target_rank == 2, "Transpose only implemented for matrix valued Green's functions"

        d = np.transpose(self.data.copy(), (0, 2, 1))
        return Gf(mesh=self.mesh, data=d)

    def conjugate(self):
        """Conjugate of the Green's function.

        Returns
        -------
        G : Gf (copy)
            Conjugate of the Green's function.
        """
        return Gf(mesh=self.mesh, data=np.conj(self.data))

    def zero(self):
        """Set all values to zero."""
        self._data[:] = 0

    def from_L_G_R(self, L, G, R):
        r"""Matrix transform of the target space of a matrix valued Green's function.

        Sets the current Green's function :math:`g_{ab}` to the matrix transform of :math:`G_{cd}`
        using the left and right transform matrices :math:`L_{ac}` and :math:`R_{db}`.

        .. math::
            g_{ab} = \sum_{cd} L_{ac} G_{cd} R_{db}

        Parameters
        ----------
        L : (a, c) ndarray
            Left side transform matrix.
        G : Gf matrix valued target_shape == (c, d)
            Green's function to transform.
        R : (d, b) ndarray
            Right side transform matrix.

        Notes
        -----
        Only implemented for Green's functions with a single mesh.
        """

        assert self.rank == 1, "Only implemented for Green's functions with one mesh"
        assert self.target_rank == 2, "Matrix transform only valid for matrix valued Green's functions"

        assert len(L.shape) == 2, 'L needs to be two dimensional'
        assert len(R.shape) == 2, 'R needs to be two dimensional'

        assert L.shape[1] == G.target_shape[0], 'Dimension mismatch between L and G'
        assert R.shape[0] == G.target_shape[1], 'Dimension mismatch between G and R'

        assert L.shape[0] == self.target_shape[0], 'Dimension mismatch between L and self'
        assert R.shape[1] == self.target_shape[1], 'Dimension mismatch between R and self'

        if not L.strides == sorted(L.strides):
            L = L.copy(order='C')

        if not R.strides == sorted(R.strides):
            R = R.copy(order='C')

        wrapped_aux._set_from_gf_data_mul_LR(self.data, L, G.data, R)

    def total_density(self, *args, **kwargs):
        """Compute the total density (trace of the density matrix).

        Parameters
        ----------
        beta : float, optional
            Inverse temperature, required only for a :class:`~triqs.mesh.meshes.MeshReFreq`
            mesh.

        Returns
        -------
        float
            Total density of the Green's function.

        Notes
        -----
        Only implemented for single-mesh Green's functions on a
        Matsubara, real-frequency or Legendre mesh.
        """
        return np.trace(gf_fnt.density(self, *args, **kwargs))

    # -----------------------------  IO  -----------------------------------

    def __reduce__(self):
        return call_factory_from_dict, (Gf, self.name, self.__reduce_to_dict__())

    def __reduce_to_dict__(self):
        d = {'mesh': self._mesh, 'data': self._data}
        return d

    _hdf5_format_ = 'Gf'

    @classmethod
    def __factory_from_dict__(cls, name, d):
        # Backward compatibility layer
        # Drop singularity from the grp and ignore it
        d.pop('singularity', None)
        # Backward compatibility layer
        # Drop indices from the grp and ignore it
        d.pop('indices', None)
        #
        r = cls(name=name, **d)
        # Backward compatibility layer
        # In the case of an ImFreq function, old archives did store only the >0
        # frequencies, we need to duplicate it for negative freq.
        # Same code as in the C++ h5_read for gf.
        need_unfold = isinstance(r.mesh, meshes.MeshImFreq) and r.mesh.positive_only
        return r if not need_unfold else wrapped_aux._make_gf_from_real_gf(r)

    # -----------------------------plot protocol -----------------------------------

    def _plot_(self, opt_dict):
        """Implement the plot protocol."""
        return plot.dispatcher(self)(self, opt_dict)

    def x_data_view(self, x_window=None, flatten_y=False):
        """Helper method for getting a view of the data.

        Parameters
        ----------
        x_window : tuple of float, optional
            The window of x variable (omega/omega_n/t/tau) for which data
            is requested.
        flatten_y : bool, optional
            If the Green's function is of size (1, 1) flatten the array as
            a 1d array.

        Returns
        -------
        (X, data) : tuple
            X is a 1d numpy array of the x variable inside the window
            requested. data is a 3d numpy array of dim ``(:, :, len(X))``,
            the corresponding slice of data. If ``flatten_y`` is True and
            dim is ``(1, 1, *)`` it returns a 1d numpy array.
        """

        if isinstance(self.mesh, (meshes.MeshReFreq, meshes.MeshImTime)):
            X = self.mesh.values()
        elif isinstance(self.mesh, meshes.MeshImFreq):
            X = np.vectorize(lambda x: x.imag)(self.mesh.values())
        else:
            raise AttributeError('input mesh must be either MeshReFreq, MeshImFreq, or MeshImTime')

        X, data = np.array(X), self.data
        if x_window:
            # the slice due to clip option x_window
            sl = clip_array(X, *x_window) if x_window else slice(len(X))
            X, data = X[sl], data[sl, :, :]
        if flatten_y and data.shape[1:3] == (1, 1):
            data = data[:, 0, 0]
        return X, data


# ---------------------------------------------------------

from h5.formats import register_class, register_backward_compatibility_method

register_class(Gf)


# A backward compatility function
def bckwd(hdf_scheme):
    """Map a legacy ``GfXxx`` HDF5 scheme name to the constructor kwargs for the current ``Gf`` class.

    The legacy scheme name has the form ``GfM1[_x_M2][_s|Tv3|Tv4]`` (e.g. ``GfImFreq``,
    ``GfImTime_x_ImFreq``, ``GfReFreqTv3``). The leading ``Gf`` and any target-rank/scalar
    suffix are stripped, and the remaining mesh portion is prefixed with ``Mesh`` to yield
    the registered mesh class name expected by ``Gf.__init__``.
    """
    m, t = hdf_scheme[2:], ''  # get rid of Gf
    for suffix in ['_s', 'Tv3', 'Tv4']:
        if m.endswith(suffix):
            m, t = m[: -len(suffix)], suffix
            break
    return {'mesh': 'Mesh' + m}


# backward compatibility: suppress unrecognized GfIndices warnings
# when reading a Gf stored with triqs <3.2.x
warnings.filterwarnings('ignore', message='The hdf5 format GfIndices')

register_backward_compatibility_method('GfImFreq', 'Gf', bckwd)
register_backward_compatibility_method('GfImTime', 'Gf', bckwd)
register_backward_compatibility_method('GfLegendre', 'Gf', bckwd)
register_backward_compatibility_method('GfReFreq', 'Gf', bckwd)
register_backward_compatibility_method('GfReTime', 'Gf', bckwd)
