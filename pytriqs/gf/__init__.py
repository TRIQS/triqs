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

  >>> from pytriqs.gf.local import *
"""

from gf import *
from singularities import *
from block_gf import BlockGf
from block2_gf import Block2Gf
from descriptors import Omega, iOmega_n, SemiCircular, Wilson, Flat, Fourier, InverseFourier, LegendreToMatsubara, MatsubaraToLegendre
from tools import *

from local.gf_imfreq import *
from local.gf_imtime import *
from local.gf_refreq import *
from local.gf_retime import *

from gf_fnt import is_gf_real_in_tau

__all__ = ['Omega','iOmega_n','SemiCircular','Flat', 'Wilson','Fourier','InverseFourier','LegendreToMatsubara','MatsubaraToLegendre',
            'lazy_expressions','TailGf',
            'GfImFreq', 'MeshImFreq', 
            'GfImTime', 'MeshImTime',
            'GfReFreq', 'MeshReFreq',
            'GfReTime', 'MeshReTime',
           # 'GfBrillouinZone', 'MeshBrillouinZone',
           # 'GfLegendre', 'GfLegendreTv3', 'GfLegendreTv4', 'MeshLegendre',
           # 'make_gf_from_inverse_fourier',
           'BlockGf','Block2Gf',
           'inverse','is_gf_real_in_tau',
            'delta','dyson','tail_fit',
           # 'rebinning_tau',
           # 'multivar'
           ]
