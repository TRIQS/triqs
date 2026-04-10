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
This is the base module for all common operations with Green's functions.
It is imported with the command::

  >>> from triqs.gfs import *
"""

from triqs.mesh import MeshPoint, MeshValueGenerator, MeshProduct, MatsubaraFreq
from .gf import *
from .block_gf import BlockGf, fix_gf_struct_type
from .block2_gf import Block2Gf
from .map_block import map_block
from .descriptors import Omega, iOmega_n, SemiCircular, Wilson, Flat, Fourier, LegendreToMatsubara, MatsubaraToLegendre
from .tools import *

from .backwd_compat.gf_imfreq import *
from .backwd_compat.gf_imtime import *
from .backwd_compat.gf_refreq import *
from .backwd_compat.gf_retime import *
from .backwd_compat.gf_legendre import *

from triqs.mesh import MeshImTime, MeshImFreq, MeshLegendre, MeshDLR, MeshDLRImFreq, MeshDLRImTime, MeshReFreq, MeshReTime, MeshReFreqPts, MeshReFreqLog, MeshBrZone, MeshCycLat, make_adjoint_mesh
MeshBrillouinZone = MeshBrZone
MeshCyclicLattice = MeshCycLat

from .gf_fnt import fit_tail, fit_hermitian_tail, density, set_from_fourier, is_gf_real_in_tau, set_from_legendre, set_from_imfreq, set_from_imtime, is_gf_hermitian, fit_tail_on_window, fit_hermitian_tail_on_window, replace_by_tail, replace_by_tail_in_fit_window, rebinning_tau, enforce_discontinuity

from .gf_factories import make_gf_from_fourier, make_hermitian, make_real_in_tau, make_gf_dlr, fit_gf_dlr, make_gf_dlr_imtime, make_gf_dlr_imfreq, make_gf_imtime, make_gf_imfreq

import warnings

__all__ = [ 'Omega','iOmega_n','SemiCircular','Flat', 'Wilson','Fourier','LegendreToMatsubara','MatsubaraToLegendre',
            'lazy_expressions',
            'MeshPoint','MeshValueGenerator',
            'MatsubaraFreq',
            'Idx', 'Gf', 'MeshProduct',
            'GfImFreq', 'MeshImFreq',
            'GfImTime', 'MeshImTime',
            'GfReFreq', 'MeshReFreq', 'MeshReFreqPts', 'MeshReFreqLog',
            'GfReTime', 'MeshReTime',
            'GfLegendre', 'MeshLegendre',
            'MeshBrZone', 'MeshBrillouinZone',
            'MeshCycLat', 'MeshCyclicLattice',
            'MeshDLR', 'MeshDLRImFreq', 'MeshDLRImTime',
            'BlockGf', 'fix_gf_struct_type',
            'Block2Gf',
            'inverse', 'conjugate', 'transpose',
            'is_gf_real_in_tau',
            'delta', 'dyson',
            'map_block',
            'make_real_in_tau', 'is_gf_real_in_tau',
            'make_hermitian', 'is_gf_hermitian',
            'fit_tail', 'fit_hermitian_tail',
            'fit_tail_on_window', 'fit_hermitian_tail_on_window',
            'replace_by_tail', 'replace_by_tail_in_fit_window',
            'set_from_fourier', 'make_gf_from_fourier',
            'set_from_legendre', 'set_from_imfreq', 'set_from_imtime',
            'make_gf_dlr', 'fit_gf_dlr', 'make_gf_dlr_imtime', 'make_gf_dlr_imfreq',
            'make_gf_imtime', 'make_gf_imfreq',
            'rebinning_tau', 'enforce_discontinuity',
            'density',
            'make_adjoint_mesh',
            'make_zero_tail',
            'rebinning_tau'
          ]
