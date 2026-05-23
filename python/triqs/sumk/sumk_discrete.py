# Copyright (c) 2013-2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2017 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020-2023 Simons Foundation
# Copyright (c) 2017 Hugo U.R. Strand
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
# Authors: John Bonini, Michel Ferrero, Alexander Hampel, Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell


r"""Evaluates the momentum sum :math:`\sum_k w_k [(i\omega_n + \mu)\mathbf{1} - \epsilon_k - \Sigma(k, i\omega_n)]^{-1}`
on a :class:`triqs.gfs.BlockGf`."""

from triqs.gfs import *
import triqs.utility.mpi as mpi
from itertools import *
import inspect
import copy,numpy

class SumkDiscrete:
    r"""
    Base class for a discrete momentum sum of a lattice Green's function.

    Holds a discretized k-grid (points, weights, hoppings) and computes

    .. math::

        G(i\omega_n) = \sum_k w_k \bigl[ (i\omega_n + \mu)\,\mathbf{1}
                       - \epsilon_k - \Sigma(k, i\omega_n) \bigr]^{-1}

    for a :class:`triqs.gfs.BlockGf` whose blocks have the same matrix size as
    :math:`\epsilon_k`. The grid itself is not populated by this class: the
    arrays are allocated by :meth:`resize_arrays` and must be filled in by a
    subclass (typically :class:`SumkDiscreteFromLattice`).

    Parameters
    ----------
    dim : int
        Spatial dimension of the underlying Brillouin zone (1, 2 or 3).
    gf_struct : list
        Block structure of the Green's function: a list of block labels
        accepted by the ``G`` and ``Sigma`` arguments of :meth:`__call__`.
    orthogonal_basis : bool, optional
        Whether the orbital basis is orthonormal. Default ``True``. Non-
        orthogonal bases are not currently exercised by the implementation.

    Attributes
    ----------
    dim : int
        Spatial dimension of the Brillouin zone.
    orthogonal_basis : bool
        Whether the orbital basis is orthonormal.
    GFBlocIndices : list
        Block structure (list of block labels) accepted by :meth:`__call__`
        for its ``G`` and ``Sigma`` arguments.
    hopping : numpy.ndarray
        Complex array of shape ``(nk, n_orbitals, n_orbitals)`` holding
        :math:`\epsilon_k = t(k)` at each grid point. Allocated by
        :meth:`resize_arrays`.
    bz_points : numpy.ndarray
        Float array of shape ``(nk, dim)`` with the k-vectors in the reduced
        Brillouin zone (components in :math:`(-1/2,\,1/2)`).
    bz_weights : numpy.ndarray
        Float array of shape ``(nk,)`` with the integration weights of each
        k-point. Initialised to a uniform :math:`1/n_k`.

    Notes
    -----
    The k-loop in :meth:`__call__` is parallelised with MPI via
    :func:`triqs.utility.mpi.slice_array` and the partial results are summed
    with :func:`triqs.utility.mpi.all_reduce`. For typical use, prefer
    :class:`SumkDiscreteFromLattice`, which constructs the grid directly from
    a :class:`triqs.lattice.tight_binding.TBLattice`.
    """
    def __init__ (self, dim, gf_struct, orthogonal_basis = True ):
        r"""
        Initialise the discrete sum-k container.

        The grid arrays (:attr:`hopping`, :attr:`bz_points`,
        :attr:`bz_weights`) are not allocated here; call
        :meth:`resize_arrays` once the number of k-points is known.

        Parameters
        ----------
        dim : int
            Spatial dimension of the Brillouin zone (1, 2 or 3).
        gf_struct : list
            Block structure of the Green's function (list of block labels).
        orthogonal_basis : bool, optional
            Whether the orbital basis is orthonormal. Default ``True``.
        """
        self.__GFBLOC_Structure = copy.deepcopy(gf_struct)
        self.orthogonal_basis,self.dim = orthogonal_basis,dim

   #-------------------------------------------------------------

    def resize_arrays (self, nk):
        r"""
        (Re)allocate the k-grid arrays for ``nk`` points.

        Sets:

        * :attr:`hopping` to a zero array of shape
          ``(nk, n_orbitals, n_orbitals)``;
        * :attr:`bz_points` to a zero array of shape ``(nk, dim)``;
        * :attr:`bz_weights` to a uniform array of shape ``(nk,)``,
          normalised so that the weights sum to one.

        Their contents (other than the weights) must be filled in by the
        caller; this method does not initialise hoppings or k-vectors.

        Parameters
        ----------
        nk : int
            Total number of k-points stored on the grid.
        """
        # constructs the arrays.
        no = len(self.__GFBLOC_Structure)
        self.hopping    = numpy.zeros([nk,no,no],numpy.complex128)   # t(k_index,a,b)
        self.bz_points  = numpy.zeros([nk,self.dim],numpy.float64)      # k(k_index,:)
        self.bz_weights = numpy.ones([nk],numpy.float64)/ float(nk) # w(k_kindex) ,  default normalisation
        self.mu_pattern  =  numpy.identity(no,numpy.complex128) if self.orthogonal_basis else numpy.zeros([no,no,nk],numpy.complex128)
        self.overlap = numpy.array(self.mu_pattern, copy=True)

   #-------------------------------------------------------------

    @property
    def GFBlocIndices(self):
        """Block structure (list of block labels) accepted by :meth:`__call__`
        for its ``G`` and ``Sigma`` arguments.

        Returns
        -------
        list
            The block labels passed to the constructor as ``gf_struct``.
        """
        return self.__GFBLOC_Structure

    #-------------------------------------------------------------

    def __call__ (self, Sigma, mu=0, field=None, epsilon_hat=None, result=None, selected_blocks=()):
        r"""
        Compute the momentum-summed local Green's function.

        Evaluates

        .. math::

            G(i\omega_n) = \sum_k w_k \bigl[ (i\omega_n + \mu)\,\mathbf{1}
                           - \mathrm{field} - \hat\epsilon(\epsilon_k)
                           - \Sigma(k, i\omega_n) \bigr]^{-1}

        over the stored grid ``(bz_points, bz_weights, hopping)``, in
        parallel across MPI ranks.

        Parameters
        ----------
        Sigma : triqs.gfs.BlockGf or callable
            Either a Green's function block, or a callable returning one.
            When callable, it must accept one argument ``k`` (1D
            :class:`numpy.ndarray` of shape ``(dim,)`` with components in
            :math:`(-1/2, 1/2)`) or two arguments ``(k, eps_k)`` (with
            ``eps_k`` the ``(n_orb, n_orb)`` hopping matrix at that k).
            Each block of the result must have the same target shape as
            :attr:`hopping` (or as ``epsilon_hat(hopping[k])`` when
            ``epsilon_hat`` is given).
        mu : float, optional
            Chemical potential. Default 0.
        field : optional
            Any k-independent object subtracted from the inverse propagator
            (e.g. a matrix-shaped array or a Green's function block).
            Default ``None``.
        epsilon_hat : callable, optional
            Function mapping ``hopping[k]`` to a matrix with the same
            target shape as each block of ``Sigma``. Default ``None``
            (use ``hopping[k]`` directly).
        result : triqs.gfs.BlockGf, optional
            Pre-allocated output. If given, the calculation writes into it
            and returns the same object, enabling chained calls such as
            ``SK(mu=mu, Sigma=Sigma, result=G).total_density()``. If
            ``None`` (default), a fresh copy of the model is returned.
        selected_blocks : tuple, optional
            Reserved for future use; currently must be ``()``.

        Returns
        -------
        triqs.gfs.BlockGf
            The local Green's function on the same
            :class:`triqs.gfs.MeshImFreq` as the model (or as ``result``
            when supplied).

        Notes
        -----
        The mesh of every block in the result must be a
        :class:`triqs.gfs.MeshImFreq`. The orbital basis must be
        orthogonal (``self.orthogonal_basis == True``). The same
        :math:`t(k)` is used for every block, and the partial sums from
        each MPI rank are combined with
        :func:`triqs.utility.mpi.all_reduce` followed by a barrier.
        """

        assert selected_blocks == (), "selected_blocks not supported for now"
            #S = Sigma.view_selected_blocks(selected_blocks) if selected_blocks else Sigma
            #Gres = result if result else Sigma.copy()
            #G = Gres.view_selected_blocks(selected_blocks) if selected_blocks else Gres

        # check Sigma
        # case 1) Sigma is a BlockGf
        if isinstance(Sigma, BlockGf):
            model = Sigma
            Sigma_fnt = False
        # case 2) Sigma is a function returning a BlockGf
        else:
            assert callable(Sigma), "If Sigma is not a BlockGf it must be a function"
            Sigma_Nargs = len(inspect.getfullargspec(Sigma)[0])
            assert Sigma_Nargs <= 2, "Sigma must be a function of k or of k and epsilon"
            if Sigma_Nargs == 1:
                model = Sigma(self.bz_points[0])
            elif Sigma_Nargs == 2:
                model = Sigma(self.bz_points[0], self.hopping[0])
            Sigma_fnt = True

        G = result if result else model.copy()
        assert isinstance(G,BlockGf), "G must be a BlockGf"
        assert isinstance(G.mesh, MeshImFreq), "G.mesh must be MeshImFreq but is {}".format(type(G.mesh))

        # check input
        assert self.orthogonal_basis, "Local_G: must be orthogonal. non ortho cases not checked."
        # check that each block has the same size
        assert len(list(set([g.target_shape[0] for i,g in G]))) == 1
        assert self.bz_weights.shape[0] == self.n_kpts(), "Internal Error"
        no = list(set([g.target_shape[0] for i,g in G]))[0]
        # check that the target shape of each block matches self.hopping
        eps_hat = epsilon_hat(self.hopping[0]) if epsilon_hat else self.hopping[0]
        assert (no,no) == eps_hat.shape, (f"Target shape of each block in Sigma: {(no,no)} does not to match orbital dimension of the hopping matrix: {eps_hat.shape}.")

        # Initialize
        G.zero()
        tmp,tmp2 = G.copy(),G.copy()
        mupat = mu * numpy.identity(no, numpy.complex128)
        tmp << iOmega_n
        if field != None: tmp -= field
        if not Sigma_fnt: tmp -= Sigma  # substract Sigma once for all

        # Loop on k points...
        for w, k, eps_k in zip(*[mpi.slice_array(A) for A in [self.bz_weights, self.bz_points, self.hopping]]):

            eps_hat = epsilon_hat(eps_k) if epsilon_hat else eps_k
            tmp2 << tmp
            tmp2 -= tmp2.n_blocks * [eps_hat - mupat]

            if Sigma_fnt:
                if Sigma_Nargs == 1: tmp2 -= Sigma(k)
                elif Sigma_Nargs == 2: tmp2 -= Sigma(k,eps_k)

            tmp2.invert()
            tmp2 *= w
            G += tmp2

        G << mpi.all_reduce(G)
        mpi.barrier()

        return G

    #-------------------------------------------------------------

    def n_kpts(self):
        """Number of k-points on the stored grid.

        Returns
        -------
        int
            Length of the first axis of :attr:`bz_points`.
        """
        return self.bz_points.shape[0]
