# Copyright (c) 2013-2015 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2015 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020-2022 Simons Foundation
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
# Authors: Michel Ferrero, Alexander Hampel, Olivier Parcollet, Nils Wentzell


r"""Specialisation of :class:`SumkDiscrete` that builds the k-grid from a 
:class:`triqs.lattice.tight_binding.TBLattice`."""

from .sumk_discrete import SumkDiscrete
import numpy as np
from triqs.lattice.tight_binding import TBLattice

class SumkDiscreteFromLattice (SumkDiscrete):
    r"""
    Discrete momentum sum built from a tight-binding lattice.

    Specialises :class:`SumkDiscrete` by constructing the k-grid directly
    from a :class:`triqs.lattice.tight_binding.TBLattice`. Computes

    .. math::

        G(i\omega_n) = \sum_k w_k \bigl[ (i\omega_n + \mu)\,\mathbf{1}
                       - \epsilon_k - \Sigma(k, i\omega_n) \bigr]^{-1}

    for a :class:`triqs.gfs.BlockGf` whose blocks share the orbital
    dimension of the lattice. The grid can cover either the full Brillouin
    zone (uniform Riemann or Gauss-Legendre sampling) or a triangulated
    patch of it. The hoppings :math:`\epsilon_k = t(k)` are obtained via
    :meth:`triqs.lattice.tight_binding.TBLattice.fourier`.

    Parameters
    ----------
    lattice : triqs.lattice.tight_binding.TBLattice
        Tight-binding lattice providing the orbital basis and :math:`t(k)`.
    patch : optional
        Triangulated Brillouin-zone patch on which to sum. If ``None``
        (default), the full BZ is used.
    n_points : int, optional
        Number of grid points per spatial direction. Default 8.
    method : {"Riemann", "Gauss"}, optional
        Integration scheme. Default ``"Riemann"``. The ``"Gauss"`` branch
        is not currently functional.

    Attributes
    ----------
    SL : triqs.lattice.tight_binding.TBLattice
        The lattice supplied at construction.
    patch : object or None
        The BZ patch supplied at construction.
    method : str
        The integration scheme used to build the grid.
    """

    def __init__(self, lattice, patch = None, n_points = 8, method = "Riemann"):
        r"""
        Build the lattice-based discrete sum-k object and its k-grid.

        Parameters
        ----------
        lattice : triqs.lattice.tight_binding.TBLattice
            Tight-binding lattice providing :math:`t(k)` via
            :meth:`triqs.lattice.tight_binding.TBLattice.fourier`.
        patch : optional
            Triangulated Brillouin-zone patch. If ``None`` (default), the
            full BZ is sampled.
        n_points : int, optional
            Number of grid points per spatial direction. Default 8.
        method : {"Riemann", "Gauss"}, optional
            Integration scheme. Default ``"Riemann"``. The ``"Gauss"``
            branch is not currently functional.
        """
        assert isinstance(lattice,TBLattice), "lattice must be a TBLattice instance"
        self.SL = lattice
        self.patch,self.method = patch,method
        # init the array
        SumkDiscrete.__init__ (self, dim = self.SL.ndim, gf_struct = lattice.orbital_names)
        self.Recompute_Grid(n_points,  method)

     #-------------------------------------------------------------

    def __reduce__(self):
        """Pickle support: returns the arguments needed to reconstruct this
        instance."""
        return self.__class__,  (self.SL, self.patch, self.bz_weights.shape[0],self.method)

     #-------------------------------------------------------------

    def Recompute_Grid (self, n_points, method="Riemann", Q=None):
        r"""
        (Re)compute the k-grid on the patch (or full BZ) given at construction.

        Reallocates the grid arrays via :meth:`SumkDiscrete.resize_arrays`
        and dispatches to the patch-based or full-BZ helper depending on
        whether :attr:`patch` is set.

        Parameters
        ----------
        n_points : int
            Number of grid points per spatial direction.
        method : {"Riemann", "Gauss"}, optional
            Integration scheme. Default ``"Riemann"``. The ``"Gauss"``
            branch is not currently functional.
        Q : array-like, optional
            Momentum shift: when given, :math:`t(k + Q)` is stored instead
            of :math:`t(k)`. Useful for evaluating quantities such as the
            bare susceptibility :math:`\chi_0(Q)`. Default ``None``.
        """
        assert method in ["Riemann","Gauss"], "method %s is not recognized"%method
        self.method = method
        self.resize_arrays(n_points)
        if self.patch:
            self.__Compute_Grid_One_patch(self.patch, n_points , method, Q)
        else:
            self.__Compute_Grid(n_points, method, Q)

     #-------------------------------------------------------------

    def __Compute_Grid (self, n_bz,  method="Riemann", Q=None):
        """Internal: build a uniform k-grid over the full Brillouin zone
        (Riemann sum) and store the corresponding hoppings."""

        n_bz_A,n_bz_B, n_bz_C = n_bz, (n_bz if self.dim > 1 else 1), (n_bz if self.dim > 2 else 1)
        nk = n_bz_A* n_bz_B* n_bz_C
        self.resize_arrays(nk)

        # compute the points where to evaluate the function in the BZ and with the weights
        def pts1d(N):
            for n in range(N):
                yield (n - N/2 +1.0) / N

        if method=="Riemann":
            bz_weights=1.0/nk
            k_index =0
            for nz in pts1d(n_bz_C):
                for ny in pts1d(n_bz_B):
                    for nx in pts1d(n_bz_A):
                        self.bz_points[k_index,:] = (nx,ny,nz)[0:self.dim]
                        k_index +=1

        elif method=="Gauss":
            assert 0, "Gauss: NR gauleg not checked"
            k_index =0
            for wa,ptsa in NR.Gauleg(-pi,pi,n_bz_A):
                for wb,ptsb in NR.Gauleg(-pi,pi,n_bz_B):
                    for wc,ptsc in NR.Gauleg(-pi,pi,n_bz_C):
                        self.bz_points[k_index,:] = (ptsa,ptsb,ptsc)[0:self.dim] /(2*pi)
                        self.bz_weights[k_index] = wa * wb * wc
                        k_index +=1
        else:
            raise IndexError("Summation method unknown")

        # A shift
        if Q:
            try:
                Q = np.array(Q)
                assert len(Q.shape) ==1
            except:
                raise RuntimeError("Q is not of correct type")
            for k_index in range(self.N_kpts()):
                self.bz_points[k_index,:] +=Q

        # Compute the discretized hoppings from the Superlattice
        self.hopping[:,:,:] = self.SL.fourier(self.bz_points)

        if self.orthogonal_basis:
            self.mu_pattern[:,:] = np.identity(self.SL.n_orbitals)
        else:
            assert 0 , "not checked"
            self.overlap[:,:,:] = self.SL.Overlap(bz_points.transpose().copy())
            mupat = np.identity(self.SL.n_orbitals)
            for k_index in range(self.N_kpts()):
                self.mu_pattern[:,:,k_index] = np.dot( mupat ,self.Overlap[:,:,k_index])

    #-------------------------------------------------------------


    def __Compute_Grid_One_patch(self, patch, n_bz, method = "Riemann", Q=None):
        """Internal: build a k-grid by sampling the triangulated BZ patch
        and store the corresponding hoppings, with weights normalised to
        sum to one over the patch."""

        tritemp = np.array(patch._triangles)
        ntri = len(tritemp)/3
        nk = n_bz*n_bz*ntri
        self.resize_arrays(nk)

        # Reshape the list to group 3 points together
        triangles = tritemp.reshape((ntri,3,2))
        total_weight = 0

        # Loop over all k-points in the triangles
        k_index = 0
        for (a,b,c),w in zip(triangles,patch._weights):
          g = ((a+b+c)/3.0-a)/n_bz;
          for i in range(n_bz):
            s = i/float(n_bz)
            for j in range(n_bz-i):
              t = j/float(n_bz)
              for k in range(2):
                rv = a+s*(b-a)+t*(c-a)+(k+1)*g
                if k == 0 or j < n_bz-i-1:
                  self.bz_points[k_index]  = rv
                  self.bz_weights[k_index] = w/(n_bz*n_bz)
                  total_weight += self.bz_weights[k_index]
                  k_index = k_index+1

        # Normalize weights so that they sum up to 1
        self.bz_weights /= total_weight

        # Compute the discretized hoppings from the Superlattice
        self.hopping[:,:,:] = self.SL.fourier(self.bz_points)

        if self.orthogonal_basis:
            self.mu_pattern[:,:] =  self.SL.MuPattern[:,:]
        else:
            assert 0 , "not checked"
