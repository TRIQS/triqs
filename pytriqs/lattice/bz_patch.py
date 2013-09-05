
################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011 by M. Ferrero, O. Parcollet
#
# TRIQS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# TRIQS. If not, see <http://www.gnu.org/licenses/>.
#
################################################################################

#from pytriqs import *
import numpy
from pytriqs.dos import DOS
from pytriqs.lattice.tight_binding import dos_patch

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
