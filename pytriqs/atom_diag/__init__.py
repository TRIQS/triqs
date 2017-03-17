##############################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2016, P. Seth, I. Krivenko, M. Ferrero and O. Parcollet
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
##############################################################################

from pytriqs.operators import Operator
from atom_diag import AtomDiagReal, AtomDiagComplex
#from atom_diag import AtomBlockGf
from atom_diag import partition_function, atomic_density_matrix, trace_rho_op, act
from atom_diag import quantum_number_eigenvalues, quantum_number_eigenvalues_checked
#from atom_diag import atomic_gf

# Construct real/complex AtomDiag
def AtomDiag(*args, **kwargs):
    """Lightweight exact diagonalization solver"""

    if len(args) > 0: h = args[0]
    elif 'h' in kwargs: h = kwargs['h']
    else:
        raise RuntimeError("You must provide a Hamiltonian!")
    if not isinstance(h, Operator):
        raise RuntimeError("The Hamiltonian must be an Operator object!")

    if any(abs(term[-1].imag) > 0 for term in h):
        return AtomDiagComplex(*args, **kwargs)
    else:
        return AtomDiagReal(*args, **kwargs)

__all__ = ['AtomDiag','AtomDiagReal','AtomDiagComplex',#'AtomBlockGf',
           'partition_function','atomic_density_matrix','trace_rho_op','act',
           'quantum_number_eigenvalues','quantum_number_eigenvalues_checked',
           ]
#           'atomic_gf']
