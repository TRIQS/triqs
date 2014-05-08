
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

__all__ = ['archive', 'dmft', 'dos', 'fit', 'gf', 'lattice', 'plot', 'sumk', 'utility', 'parameters', 'wrap_test']

import sys
if 'additional_builtin_modules' in dir(sys) :

    from version import greeting
    import utility.mpi as mpi
    if (mpi.world.rank == 0) :  sys.stderr.write(greeting)


