# Copyright (c) 2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2017 Centre national de la recherche scientifique (CNRS)
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

from . import one_var
from triqs.gf.meshes import *
from triqs.gf.mesh_product import MeshProduct

def dispatcher(g) :
    """ given g, pick up the right plotting function"""

    if not isinstance(g.mesh, MeshProduct):
        m= g.mesh

        if isinstance(m, MeshImFreq) : return one_var.imfreq
        if isinstance(m, MeshImTime) : return one_var.imtime
        if isinstance(m, MeshReFreq) : return one_var.refreq
        if isinstance(m, MeshReTime) : return one_var.retime
        if isinstance(m, MeshLegendre) : return one_var.legendre

    else:
        raise NotImplemented 

__init__ = [dispatcher]
