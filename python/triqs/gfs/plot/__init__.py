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

r"""Plot-protocol back-end for Green's functions."""

from . import one_var
from triqs.mesh import *
from triqs.mesh import MeshProduct

def dispatcher(g) :
    """Return the plot function in :mod:`.one_var` matching ``g.mesh``.

    Parameters
    ----------
    g : Gf
        Green's function being plotted.

    Returns
    -------
    callable
        ``f(g, opt_dict) -> list of dict`` returning per-curve plot
        data in the format expected by
        :func:`~triqs.plot.mpl_interface.oplot`.

    Raises
    ------
    NotImplementedError
        If ``g.mesh`` is a :class:`~triqs.mesh.mesh_product.MeshProduct` 
        (multi-variable plots are not handled here).
    """

    if not isinstance(g.mesh, MeshProduct):
        m= g.mesh

        if isinstance(m, MeshImFreq) : return one_var.imfreq
        if isinstance(m, MeshImTime) : return one_var.imtime
        if isinstance(m, MeshDLRImFreq) : return one_var.dlr_imfreq
        if isinstance(m, MeshDLRImTime) : return one_var.dlr_imtime
        if isinstance(m, MeshReFreq) : return one_var.refreq
        if isinstance(m, MeshReFreqPts) : return one_var.refreq
        if isinstance(m, MeshReFreqLog) : return one_var.refreq
        if isinstance(m, MeshReTime) : return one_var.retime
        if isinstance(m, MeshLegendre) : return one_var.legendre

    else:
        raise NotImplementedError

__init__ = [dispatcher]
