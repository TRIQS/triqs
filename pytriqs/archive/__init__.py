################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011 by M. Ferrero, O. Parcollet
# Copyright (C) 2020 Simons Foundation
#    authors: N. Wentzell
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
Deprecated module. Use
from h5 import XXX
"""

import warnings
warnings.warn("""
***************************************************

from pytriqs.archive import XXX

is deprecated. Replace it by

from h5 import XXX")

****************************************************""")

from h5 import HDFArchive, HDFArchiveGroup, HDFArchiveInert
__all__ = ['HDFArchive', 'HDFArchiveGroup', 'HDFArchiveInert']
