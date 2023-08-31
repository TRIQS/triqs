# Copyright (c) 2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2017 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020-2022 Simons Foundation
# Copyright (c) 2017 Igor Krivenko
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
# Authors: Michel Ferrero, Igor Krivenko, Nils Wentzell


from triqs.operators import Operator
from .atom_diag import AtomDiagReal, AtomDiagComplex
from .atom_diag import partition_function, atomic_density_matrix, trace_rho_op, act
from .atom_diag import quantum_number_eigenvalues, quantum_number_eigenvalues_checked
from .atom_diag import atomic_g_tau, atomic_g_iw, atomic_g_l, atomic_g_w

# Construct real/complex AtomDiag
def AtomDiag(*args, **kwargs):
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

AtomDiag.__doc__ = AtomDiagReal.__doc__.replace("Lightweight exact diagonalization solver (Real)",
                                                "Create and return an exact diagonalization solver.\nDepending on the type of h returns :py:class:`AtomDiagReal <triqs.atom_diag.atom_diag.AtomDiagReal>` or :py:class:`AtomDiagComplex <triqs.atom_diag.atom_diag.AtomDiagComplex>`")

__all__ = ['AtomDiag','AtomDiagReal','AtomDiagComplex',
           'partition_function','atomic_density_matrix','trace_rho_op','act',
           'quantum_number_eigenvalues','quantum_number_eigenvalues_checked',
           'atomic_g_tau','atomic_g_iw','atomic_g_l','atomic_g_w']
