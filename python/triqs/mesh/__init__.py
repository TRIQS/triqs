# Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2023 Simons Foundation
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

r"""
Practical representations of function domains.

Meshes form the basis for the Green's function containers in :mod:`triqs.gfs`:
they fix the mathematical domain of the underlying function, its in-memory
representation and how it is evaluated. 

A mesh consists of :math:`N` mesh points (see 
:class:`MeshPoint <mesh_point.MeshPoint>`), and each mesh point carries:

- a **data index** :math:`d \in \{0, \dots, N-1\}` (a tuple of such indices for
  product meshes) -- the position used to address the corresponding value in
  the underlying data array of a Green's function;
- a **mesh-native index** :math:`n` whose meaning depends on the
  mesh -- e.g. the Matsubara index :math:`n \in \mathbb{Z}` for 
  :class:`MeshImFreq <meshes.MeshImFreq>`, the integer triple 
  :math:`\mathbf{n} \in \mathbb{Z}^3` for :class:`MeshCycLat <meshes.MeshCycLat>`,
  or simply the index :math:`n = d` for :class:`MeshImTime <meshes.MeshImTime>`;
- an optional **value** -- e.g. Matsubara frequencies :math:`i\omega_n` for 
  :class:`MeshImFreq <meshes.MeshImFreq>`, real-space lattice vectors 
  :math:`\mathbf{R}^{\mathbf{n}}` for :class:`MeshCycLat <meshes.MeshCycLat>`, 
  imaginary time points :math:`\tau_n \in [0, \beta]` for 
  :class:`MeshImTime <meshes.MeshImTime>`.

For all C++-wrapped mesh classes, a :class:`MeshPoint <mesh_point.MeshPoint>` 
can be accessed in two equivalent ways:

- ``mp = mesh(n)`` -- function-call syntax, where ``n`` is the **mesh-native
  index**.
- ``mp = mesh[d]`` -- subscript syntax, where ``d`` is the **data index**
  :math:`d \in \{0, \dots, N-1\}`.

Furthermore, the following useful statements hold for all meshes types: 

- ``for mp in mesh: ...`` iterates over the mesh points in data-index order, 
- ``len(mesh)`` returns the size :math:`N`,
- ``mesh.to_index(d)`` / ``mesh.to_data_index(n)`` / ``mesh.to_value(n)``
  perform explicit index / value conversions, and
- ``mesh.values()`` returns an array of all mesh-point values at once.

The mesh classes wrapped from C++ live in :mod:`triqs.mesh.meshes` and are
re-exported at the package level. 

A handful of small pure-Python helpers complete the public API: 

- :class:`MeshPoint <mesh_point.MeshPoint>` represents a single mesh point 
- :class:`MeshValueGenerator <mesh_point.MeshValueGenerator>` iterates over
  mesh points and yields their values on the fly,
- :class:`MeshProduct <mesh_product.MeshProduct>` represents a cartesian 
  product of meshes for multi-variable Green's functions, and 
- :class:`MatsubaraFreq <matsubara_freq.MatsubaraFreq>` is a stand-alone 
  Matsubara frequency type usable outside a mesh context.
"""

from .meshes import (
    MeshImTime,
    MeshImFreq,
    MeshDLR,
    MeshDLRImTime,
    MeshDLRImFreq,
    MeshDLR2D,
    MeshDLR2DImFreq,
    MeshReTime,
    MeshReFreq,
    MeshReFreqLog,
    MeshReFreqPts,
    MeshCycLat,
    MeshBrZone,
    MeshChebyshev,
    MeshLegendre,
    EnergyT,
    make_adjoint_mesh,
)
from .mesh_point import MeshPoint, MeshValueGenerator
from .mesh_product import MeshProduct
from .matsubara_freq import MatsubaraFreq

__all__ = [
    # meshes (C++ wrapped)
    "MeshImTime",
    "MeshImFreq",
    "MeshDLR",
    "MeshDLRImTime",
    "MeshDLRImFreq",
    "MeshDLR2D",
    "MeshDLR2DImFreq",
    "MeshReTime",
    "MeshReFreq",
    "MeshReFreqLog",
    "MeshReFreqPts",
    "MeshCycLat",
    "MeshBrZone",
    "MeshChebyshev",
    "MeshLegendre",
    "EnergyT",
    "make_adjoint_mesh",
    # mesh_point
    "MeshPoint",
    "MeshValueGenerator",
    # mesh_product
    "MeshProduct",
    # matsubara_freq
    "MatsubaraFreq",
]
