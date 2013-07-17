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

from gf import TailGf

from inverse import inverse
from gf_imfreq import GfImFreq
from gf_imtime import GfImTime
from gf_refreq import GfReFreq
from gf_retime import GfReTime
from gf_two_real_times import GfTwoRealTime
from gf_legendre import GfLegendre
from block_gf import BlockGf
from descriptors import Omega, iOmega_n, SemiCircular, Wilson, Fourier, InverseFourier, LegendreToMatsubara, MatsubaraToLegendre

__all__ = ['Omega','iOmega_n','SemiCircular','Wilson','Fourier','InverseFourier','LegendreToMatsubara','MatsubaraToLegendre','lazy_expressions','TailGf','GfImFreq','GfImTime','GfReFreq','GfReTime','GfLegendre','BlockGf','inverse','GfTwoRealTime']

