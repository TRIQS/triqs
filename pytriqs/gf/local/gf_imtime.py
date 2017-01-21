################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2017 by M. Ferrero, O. Parcollet
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
import warnings, numpy as np
from pytriqs.gf.gf import Gf
from pytriqs.gf.mesh import MeshImTime
from pytriqs.gf.singularities import TailGf

class GfImTime(Gf) : 
    """
    Parameters (KEYWORD argument ONLY)
    ----------
    mesh: MeshImTime, optional
          The mesh of the Green function
          If not present, it will be constructed from 
          the parameters beta, indices, [n_points], [statistic]

    data: numpy.array, optional
          The data of the Gf.
          Must be of dimension mesh.rank + target_rank.
          Incompatible with target_shape

    target_shape: list of int, optional
                  Shape of the target space.
                  Incompatible with data

    is_real: bool
             Is the Green function real valued ?
             If true, and target_shape is set, the data will be real.
             No effect with the parameter data.

    singularity or tail : TailGfXXX, optional
                 One of the singularity of the module 'singularities'
                 If not present singularity is None.

    indices: GfIndices or list of str or list of list of str, Optional
             Optional string indices for the target space, to allow e.g g['eg', 'eg']
             list of list of str: the list of indices for each dimension.
             list of str: all indices are assumed to be the same for all dimensions.
   
    name: str
          The name of the Green function. For plotting.

    """
    def __init__(self, **kw): # enforce keyword only policy 
        """
          Same as Gf, but can rebuild the  mesh for backward compatibility
        """
        def delegate(self, mesh=None, data = None, target_shape=None, singularity = None, tail = None, indices = None, name='g', n_points = 10000, beta = None, statistic = 'Fermion'):
            if mesh is None:
                assert isinstance(beta, (int, long, float)), "If the Mesh is not given, beta is mandatory and must be float"
                assert isinstance(n_points, int) and n_points >0, "n_points is crazy"
                mesh = MeshImTime(beta, statistic, n_points)
           
            super(GfImTime, self).__init__(
                      mesh = mesh, 
                      data = data, 
                      target_shape = target_shape,
                      singularity = tail or singularity,
                      _singularity_maker =(lambda se : TailGf(target_shape = se.target_shape, n_order=10, order_min = -2)) if not (tail or singularity) else None,
                      indices = indices, 
                      name = name) 
        delegate(self, **kw)

    @property
    def N1(self):
        assert self.target_rank == 2, "N1 only makes sense for rank 2 targets"
        warnings.warn("g.N1 is deprecated and not generic. Use g.target_shape[0] instead")
        return self.target_shape[0]

    @property
    def N2(self):
        assert self.target_rank == 2, "N2 only makes sense for rank 2 targets"
        warnings.warn("g.N2 is deprecated and not generic. Use g.target_shape[1] instead")
        return self.target_shape[1]

    @property
    def indicesL(self):
        warnings.warn("g.indicesL is deprecated. Use g.indices[0] instead")
        return self.indices.data[0]

    @property
    def indicesR(self):
        warnings.warn("g.indicesR is deprecated. Use g.indices[1] instead")
        return self.indices.data[1]
