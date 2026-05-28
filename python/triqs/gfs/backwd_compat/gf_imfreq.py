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

r"""Deprecated :class:`~triqs.gfs.backwd_compat.gf_imfreq.GfImFreq` alias 
for :class:`~triqs.gfs.gf.Gf` with a :class:`~triqs.mesh.meshes.MeshImFreq` 
mesh."""

import warnings, numpy as np
from triqs.gfs.gf import Gf
from triqs.mesh import MeshImFreq

class GfImFreq(Gf) :
    r"""Deprecated backward-compatibility alias for :class:`~triqs.gfs.gf.Gf` 
    with a :class:`~triqs.mesh.meshes.MeshImFreq` mesh.

    Use ``Gf(mesh=MeshImFreq(...), target_shape=...)`` instead.
    Constructing a :class:`~triqs.gfs.backwd_compat.gf_imfreq.GfImFreq` 
    emits a :class:`FutureWarning`.

    Parameters
    ----------
    mesh : MeshImFreq, optional
        Mesh of the Green's function. If omitted, built from
        ``beta`` / ``n_points`` / ``statistic``.
    data : numpy.ndarray, optional
        Storage of shape ``(*mesh_sizes, *target_shape)``. Mutually
        exclusive with ``target_shape``.
    target_shape : list of int, optional
        Shape of the target space. Mutually exclusive with ``data``.
    beta : float, optional
        Inverse temperature, used to build the mesh.
    n_points : int, optional
        Number of Matsubara frequencies. Default ``1025``.
    statistic : {'Fermion', 'Boson'}, optional
        Statistic for the implicit mesh. Default ``'Fermion'``.
    is_real : bool, optional
        Allocate real-valued storage. No effect when ``data`` is given.
    name : str, optional
        Plot label.
    """
    def __init__(self, **kw): # enforce keyword only policy
        warnings.warn("Please use Gf(mesh=MeshImFreq(..), ..) instead of GfImFreq", FutureWarning)

        def delegate(self, mesh=None, data = None, target_shape=None, name='', n_points = 1025, beta = None, statistic = 'Fermion', indices=None):
            if mesh is None:
                assert isinstance(beta, (int, float)), "If the Mesh is not given, beta is mandatory and must be float"
                assert isinstance(n_points, int) and n_points >0, "n_points is crazy"
                mesh = MeshImFreq(beta, statistic, n_points)
            assert isinstance(mesh, MeshImFreq), "GfImFreq requires mesh to be of type MeshImFreq"
        
            super(GfImFreq, self).__init__(
                      mesh = mesh, 
                      data = data, 
                      target_shape = target_shape,
                      name = name,
                      indices = indices) 

        delegate(self, **kw)
