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

r"""
This is the base module for all common operations with Green's functions.
It is imported with the command::

  >>> from pytriqs.gf import *
"""

from mesh_point import MeshPoint, MeshValueGenerator
from gf import *
from block_gf import BlockGf
from block2_gf import Block2Gf
from map_block import map_block
from descriptors import Omega, iOmega_n, SemiCircular, Wilson, Flat, Fourier, InverseFourier, LegendreToMatsubara, MatsubaraToLegendre
from tools import *

from backwd_compat.gf_imfreq import *
from backwd_compat.gf_imtime import *
from backwd_compat.gf_refreq import *
from backwd_compat.gf_retime import *
from backwd_compat.gf_legendre import *

from meshes import MeshBrillouinZone, MeshCyclicLattice

from gf_fnt import fit_tail, fit_hermitian_tail, density, make_adjoint_mesh, set_from_fourier, set_from_inverse_fourier, make_real_in_tau, is_gf_real_in_tau, set_from_fourier, make_gf_from_fourier, make_gf_from_inverse_fourier, set_from_legendre, set_from_imfreq, set_from_imtime, make_hermitian, is_gf_hermitian, fit_tail_on_window, fit_hermitian_tail_on_window, replace_by_tail, replace_by_tail_in_fit_window, rebinning_tau, enforce_discontinuity, GfIndices

__all__ = [ 'Omega','iOmega_n','SemiCircular','Flat', 'Wilson','Fourier','InverseFourier','LegendreToMatsubara','MatsubaraToLegendre',
            'lazy_expressions',
            'MeshPoint','MeshValueGenerator',
            'Idx', 'Gf', 'MeshProduct', 'GfIndices',
            'GfImFreq', 'MeshImFreq', 
            'GfImTime', 'MeshImTime',
            'GfReFreq', 'MeshReFreq',
            'GfReTime', 'MeshReTime',
            'GfLegendre', 'MeshLegendre',
            'MeshBrillouinZone',
            'MeshCyclicLattice',
            'BlockGf',
            'Block2Gf',
            'inverse', 'conjugate', 'transpose',
            'is_gf_real_in_tau',
            'make_gf_from_fourier',
            'make_gf_from_inverse_fourier',
            'delta','dyson',
            'map_block',
            'make_real_in_tau', 'is_gf_real_in_tau',
            'make_hermitian', 'is_gf_hermitian',
            'fit_tail', 'fit_hermitian_tail',
            'fit_tail_on_window', 'fit_hermitian_tail_on_window',
            'replace_by_tail', 'replace_by_tail_in_fit_window',
            'set_from_fourier', 'set_from_inverse_fourier',
            'make_gf_from_fourier', 'make_gf_from_inverse_fourier',
            'set_from_legendre', 'set_from_imfreq', 'set_from_imtime',
            'rebinning_tau','enforce_discontinuity',
            'density',
            'make_adjoint_mesh',
            'make_zero_tail',
            'rebinning_tau'
          ]
