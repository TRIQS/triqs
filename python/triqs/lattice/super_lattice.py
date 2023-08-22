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


import numpy
from .tight_binding import TBLattice

__all__ = ['TBSuperLattice']

class TBSuperLattice(TBLattice):
    r""" Builds a superlattice on top of a base TBLattice.

    Parameters
    ----------

    tb_lattice : TBLattice instance
        The base tight binding lattice.

    super_lattice_units : ndarray (2D)
        The unit vectors of the superlattice in the ``tb_lattice`` (integer) coordinates.

    cluster_sites :
        Coordinates of the cluster in tb_lattice coordinates.
        If ``None``, an automatic computation of cluster positions
        is made as follows: it takes all points whose coordinates
        in the basis of the superlattice are in [0, 1[^dimension.

    remove_internal_hoppings : bool
        If ``true``, the hopping terms are removed inside the cluster.
        Useful to add Hartree Fock terms at the boundary of a cluster, e.g.

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
        assert Ncluster_sites >0, "Superlattice vectors are not independant !"
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
        """ Input: a function  r-> f(r) on the tb_lattice given as a dictionnary
            Output: the function R-> F(R) folded on the superlattice.
            Only requirement is that f(r)[orbital1, orbital2] is properly defined.
            Hence f(r) can be a numpy, a GFBloc, etc...
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
        """Given a point in the supercell R, site (number) alpha, it computes its position on the tb_lattice in lattice coordinates"""
        return numpy.dot (self._M, numpy.array(R)) + self.__cluster_sites[alpha,:]

    def change_coordinates_L_to_SL(self, x):
        """Given a point on the tb_lattice in lattice coordinates, returns its coordinates (R, alpha) in the Superlattice"""
        aux  = numpy.dot(self._Mtilde, numpy.array(x))
        R = aux // self.Ncluster_sites
        dx = list (x - numpy.dot (self._M, R) ) # force int ?
        return tuple(R), self.__cluster_sites.index(dx)

    def pack_index_site_orbital(self, n_site, n_orbital):
        """ nsite and n_orbital must start at 0"""
        return n_site + (n_orbital ) * self.Ncluster_sites

    def unpack_index_site_orbital (self, index):
        """Inverse of pack_index_site_orbital"""
        n_orbital  =   (index)//self.Ncluster_sites
        n_site =  index - n_orbital*self.Ncluster_sites

        return n_site, n_orbital

    def cluster_sites(self):
        """
           Generate the position of the cluster site in the tb_lattice coordinates.
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
