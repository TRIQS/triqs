# Copyright (c) 2013 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020-2023 Simons Foundation
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
# Authors: Michel Ferrero, Jonathan Karp, Olivier Parcollet, Hugo U.R. Strand, Nils Wentzell

"""Superlattices on top of a :class:`TBLattice`.

Provides :class:`TBSuperLattice`, which builds a tight-binding superlattice from
a base :class:`TBLattice` and an integer superlattice basis. The base-lattice
hoppings are folded onto the superlattice; internal hoppings can optionally be
removed, e.g. when adding Hartree-Fock terms at a cluster boundary.
"""

import numpy
from .tight_binding import TBLattice

__all__ = ['TBSuperLattice']

class TBSuperLattice(TBLattice):
    r"""A tight-binding superlattice built on top of a base :class:`TBLattice`.

    Parameters
    ----------
    tb_lattice : TBLattice
        The base tight-binding :class:`TBLattice`.
    super_lattice_units : numpy.ndarray
        Two-dimensional ``(ndim, ndim)`` array giving the superlattice basis
        vectors in the ``tb_lattice`` (integer) coordinates.
    cluster_sites : list of array-like of int, optional
        Coordinates of the cluster in ``tb_lattice`` coordinates. If ``None``,
        cluster positions are computed automatically as all points whose
        coordinates in the superlattice basis lie in ``[0, 1)`` along each
        dimension.
    remove_internal_hoppings : bool, default False
        If ``True``, hopping terms inside the cluster are removed. Useful
        e.g. when adding Hartree-Fock terms at a cluster boundary.

    Attributes
    ----------
    Norb : int
        Total number of orbitals in the supercell (``n_orbitals * Ncluster_sites``).
    Ncluster_sites : int
        Number of sites in the cluster.
    """
    
    def __init__(self, tb_lattice, super_lattice_units, cluster_sites = None, remove_internal_hoppings = False):

        if not isinstance(tb_lattice, TBLattice): raise ValueError("tb_lattice should be an instance of TBLattice")
        self.__BaseLattice = tb_lattice
        ndim = tb_lattice.ndim

        try:
            self.__super_lattice_units = numpy.array(super_lattice_units, copy=True)
            assert self.__super_lattice_units.shape == (ndim, ndim)
        except:
            raise ValueError("super_lattice_units is not correct. Cf Doc. value is %s, ndim = %s "%(super_lattice_units,ndim))

        Ncluster_sites = int(numpy.rint(abs(numpy.linalg.det(self.__super_lattice_units ))))
        assert Ncluster_sites >0, "Superlattice vectors are not independent !"
        self._M = self.__super_lattice_units.transpose()
        self._Mtilde = numpy.array(numpy.rint(numpy.linalg.inv(self._M)*Ncluster_sites), dtype = int)

        self.__remove_internal_hoppings = remove_internal_hoppings
        self.Norb = tb_lattice.n_orbitals * Ncluster_sites

        # cluster_sites computation
        if cluster_sites!=None:
            self.__cluster_sites = list(cluster_sites)[:]
        else: # Computes the position of the cluster automatically
            self.__cluster_sites = []
            #We tile the super-cell with the tb_lattice points and retains
            # the points inside it and store it.
            if ndim==1:  a=(max(self._M[0,:]), 0, 0 )
            elif ndim==2:  a=(2*max(self._M[0,:]), 2*max(self._M[1,:]), 0 )
            elif ndim==3: a= (3*max(self._M[0,:]), 3*max(self._M[1,:]), 3*max(self._M[2,:]))
            else: raise ValueError("ndim is not between 1 and 3 !!")
            r = lambda i:  list(range(-a[i] , a[i]+1))
            for nx in r(0):
                for ny in r(1):
                    for nz in r(2):
                        nv = numpy.array([nx, ny, nz][0:ndim])
                        k_sl = numpy.dot(self._Mtilde, nv)
                        if (min(k_sl) >= 0) and (max(k_sl) < Ncluster_sites ): # The point is a point of the cluster. We store it.
                            self.__cluster_sites.append(nv.tolist())

        assert len(self.__cluster_sites) == Ncluster_sites, """Number of cluster positions incorrect (compared to the volume of unit cell of the Superlattice)"""
        self.Ncluster_sites = Ncluster_sites

        # Compute the new hoppings in the supercell
        hoppings = self.fold(tb_lattice.hoppings, remove_internal_hoppings)
        if 0:
            for k, v in list(hoppings.items()):
                print(k)
                print(v.real)

        # Compute the new units of the lattice in real coordinates
        units = numpy.dot(self.__super_lattice_units, tb_lattice.units)

        # Positions and names of orbitals in the supercell: just translate all orbitals for cluster site positions
        # in R^3 coordinates.
        orbital_positions = [POS + tb_lattice.lattice_to_real_coordinates(CS) for POS in tb_lattice.orbital_positions for CS in self.__cluster_sites]

        #orbital_names = [ '%s%s'%(n, s) for n in tb_lattice.OrbitalNames for s in range(Ncluster_sites)]
        site_index_list, orbital_index_list = list(range(1, Ncluster_sites+1)), tb_lattice.orbital_names
        if len(orbital_index_list)==1:
            orbital_names= [ str(s) for s in site_index_list ]
        elif len(site_index_list)==1 and len(orbital_index_list)>1:
            orbital_names= [ o for o in orbital_index_list]
        elif len(site_index_list)>1 and len(orbital_index_list)>1:
            orbital_names= [ "{}_{}".format(pos, o) for o in orbital_index_list for pos in site_index_list]

        TBLattice.__init__(self, units, hoppings, orbital_positions, orbital_names)
        assert self.Norb == self.n_orbitals


    __HDF_reduction__ = ['__BaseLattice', '__super_lattice_units', '__cluster_sites', '__remove_internal_hoppings']

    def __reduce__ (self):
        return tuple([getattr(self, x) for x in self.__HDF_reduction__])

    def fold(self, D1, remove_internal=False, create_zero = None):
        """Fold a real-space function defined on the base lattice onto the superlattice.

        Parameters
        ----------
        D1 : dict
            Dictionary mapping base-lattice displacements ``r`` to an array-like
            ``f(r)`` indexed by ``[orbital1, orbital2]`` (numpy array, GfBloc, ...).
        remove_internal : bool, default False
            If True, drop the on-site (``R = 0``) block of the folded result, useful
            for adding Hartree-Fock terms at the cluster boundary.
        create_zero : callable, optional
            Factory returning a zero entry of the same type as ``f(r)``. Defaults to
            a complex numpy array of shape ``(Norb, Norb)``.

        Returns
        -------
        dict
            Folded function ``F(R)`` on the superlattice displacements.
        """
        #Res , norb = {} , self.__BaseLattice.n_orbitals
        Res , norb = {} , len(list(D1.values())[0])
        pack = self.pack_index_site_orbital
        for nsite, CS in enumerate(self.__cluster_sites):
            for disp, t in list(D1.items()):
                R, alpha = self.change_coordinates_L_to_SL(numpy.array(CS)+numpy.array(disp))
                if R not in Res: Res[R] = create_zero() if create_zero else numpy.zeros((self.Norb, self.Norb), dtype = type(t[0,0]))
                if not(remove_internal) or R!= self.tb_lattice.ndim*(0, ):
                    for orb1 in range(norb):
                        for orb2 in range(norb):
                            Res[R][pack(nsite, orb1), pack(alpha, orb2)] += t[orb1, orb2]
        return Res

    def change_coordinates_SL_to_L(self, R , alpha):
        """Map a superlattice point ``(R, alpha)`` to its position on the base lattice in lattice coordinates.

        Parameters
        ----------
        R : array-like of int
            Superlattice displacement.
        alpha : int
            Index of the site within the cluster.

        Returns
        -------
        numpy.ndarray
            Position on the base lattice in lattice coordinates.
        """
        return numpy.dot (self._M, numpy.array(R)) + self.__cluster_sites[alpha,:]

    def change_coordinates_L_to_SL(self, x):
        """Map a base-lattice point ``x`` (in lattice coordinates) to its superlattice coordinates ``(R, alpha)``.

        Parameters
        ----------
        x : array-like of int
            Position on the base lattice in lattice coordinates.

        Returns
        -------
        R : tuple of int
            Superlattice displacement.
        alpha : int
            Index of the site within the cluster.
        """
        aux  = numpy.dot(self._Mtilde, numpy.array(x))
        R = aux // self.Ncluster_sites
        dx = list (x - numpy.dot (self._M, R) ) # force int ?
        return tuple(R), self.__cluster_sites.index(dx)

    def pack_index_site_orbital(self, n_site, n_orbital):
        """Combine a (site, orbital) index pair into a single superlattice orbital index.

        Both ``n_site`` and ``n_orbital`` are zero-based.

        Parameters
        ----------
        n_site : int
            Zero-based index of the site within the cluster.
        n_orbital : int
            Zero-based orbital index on that site.

        Returns
        -------
        int
            Combined superlattice orbital index.
        """
        return n_site + (n_orbital ) * self.Ncluster_sites

    def unpack_index_site_orbital (self, index):
        """Split a superlattice orbital index back into a ``(n_site, n_orbital)`` pair (inverse of :meth:`pack_index_site_orbital`).

        Parameters
        ----------
        index : int
            Combined superlattice orbital index.

        Returns
        -------
        n_site : int
            Zero-based index of the site within the cluster.
        n_orbital : int
            Zero-based orbital index on that site.
        """
        n_orbital  =   (index)//self.Ncluster_sites
        n_site =  index - n_orbital*self.Ncluster_sites

        return n_site, n_orbital

    def cluster_sites(self):
        """Yield each cluster-site position in base-lattice coordinates.

        Yields
        ------
        list of int
            Position of a cluster site in base-lattice coordinates.
        """
        for pos in self.__cluster_sites:
            yield pos

    def __repr__(self):
        def f(A):
            return list([ tuple(x) for x in A])
        return """SuperLattice class: \n
   Base TBLattice: %s
   SuperLattice units: %s
   Remove internal hoppings: %s
   Cluster site positions: %s"""%(self.__BaseLattice, f(self.__super_lattice_units), self.__remove_internal_hoppings, self.__cluster_sites)
