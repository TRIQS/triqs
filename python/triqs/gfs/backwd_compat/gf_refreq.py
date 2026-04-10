# Copyright (c) 2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2017 Centre national de la recherche scientifique (CNRS)
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
# Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

import warnings, numpy as np
from triqs.gf.gf import Gf
from triqs.gf.meshes import MeshReFreq

class GfReFreq(Gf) : 
    """
    Parameters (KEYWORD argument ONLY)
    ----------
    mesh: MeshReFreq, optional
          The mesh of the Green function
          If not present, it will be constructed from 
          the parameters beta, [n_points], [statistic]

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

    name: str
          The name of the Green function. For plotting.

    """
    def __init__(self, **kw): # enforce keyword only policy 
        """
          Same as Gf, but can rebuild the  mesh for backward compatibility
        """
        warnings.warn("Please use Gf(mesh=MeshReFreq(..), ..) instead of GfReFreq", DeprecationWarning)

        def delegate(self, mesh=None, data = None, target_shape=None, name='', n_points = 10000, window = None, indices=None):
            if mesh is None:
                assert isinstance(n_points, int) and n_points >0, "n_points is crazy"
                mesh = MeshReFreq(window[0], window[1], n_points)
            assert isinstance(mesh, MeshReFreq), "GfReFreq requires mesh to be of type MeshReFreq"
           
            super(GfReFreq, self).__init__(
                      mesh = mesh, 
                      data = data, 
                      target_shape = target_shape,
                      name = name,
                      indices = indices) 
        delegate(self, **kw)
