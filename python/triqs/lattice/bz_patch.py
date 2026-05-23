# Copyright (c) 2013 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020 Simons Foundation
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

"""Polygonal patches of the Brillouin zone."""

#from triqs import *
import numpy
from triqs.dos import DOS
from triqs.lattice.tight_binding import dos_patch

class BZPatch:
    """A polygonal patch of the Brillouin zone, triangulated for use with :func:`triqs.lattice.tight_binding.dos_patch`.

    The input polygons are triangulated internally (one triangle per consecutive
    triple of vertices, fan-style from the first vertex), and the patch area is
    computed as the sum of triangle areas.

    Parameters
    ----------
    name : str
        Identifier for the patch (used to label downstream DOS results).
    polygons : list of list of pairs of floats
        List of polygons, each given as an ordered list of 2D vertices in the
        Brillouin zone.

    Attributes
    ----------
    name : str
        Identifier for the patch.
    weight : float
        Total area of the patch (sum of triangle areas).
    """
    
    def __init__(self, name, polygons):
        # Cut the patch in triangles (this is what is asked by the C-code)
        self.weight, self.name = 0, name
        self._triangles = []
        self._weights = []
        for polygon in polygons:
            pnt = [0, 0, 0]
            for np, point in enumerate(polygon):
                if np > 1:
                    pnt[2] = point
                    self._triangles += pnt
                    self._weights += [ 0.5*abs((pnt[1][0]-pnt[0][0])*(pnt[2][1]-pnt[0][1])
                                        -(pnt[1][1]-pnt[0][1])*(pnt[2][0]-pnt[0][0])) ]
                    self.weight += 0.5*abs((pnt[1][0]-pnt[0][0])*(pnt[2][1]-pnt[0][1])
                                      -(pnt[1][1]-pnt[0][1])*(pnt[2][0]-pnt[0][0]))
                    pnt[1] = pnt[2]
                else:
                    pnt[np%3] = point

    def dos(self, TB, n_eps, n_div):
        """Compute the partial density of states of a tight-binding Hamiltonian on this patch.

        Parameters
        ----------
        TB : TightBinding
            The tight-binding Hamiltonian.
        n_eps : int
            Number of energy bins.
        n_div : int
            Number of sub-divisions of each triangle used for the sampling.

        Returns
        -------
        triqs.dos.DOS
            Partial density of states summed over orbitals on the patch.
        """
        return dos_patch(TB, numpy.array(self._triangles), n_eps, n_div, self.name)
