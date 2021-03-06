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


#from triqs import *
import numpy
from triqs.dos import DOS
from triqs.lattice.tight_binding import dos_patch

class BZPatch:
    """Description of a Patch of the BZ"""
    def __init__(self, name, polygons):
        """ TO BE WRITTEN: MICHEL! """
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
        """ Compute the partial dos of the Patch for the Lattice lattice"""
        return dos_patch(TB, numpy.array(self._triangles), n_eps, n_div, self.name)
