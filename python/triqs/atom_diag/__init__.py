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

r"""Exact diagonalization of finite fermionic Hamiltonians and atomic Green's functions.

This module performs exact diagonalization of a many-body Hamiltonian
:math:`\hat H` acting on the Fock space of a finite set of fermionic
single-particle states. The Hilbert space is split into invariant subspaces of
:math:`\hat H`, each of which is diagonalized independently:

.. math::
   H_B = U_B \, \mathrm{diag}(E_B) \, U^{\dagger}_B \; ,

where :math:`B` indexes the subspaces, :math:`H_B` is the Hamiltonian
restricted to the subspace, :math:`U_B` is the unitary matrix mapping the Fock
basis of the subspace to the eigenbasis of the Hamiltonian, and :math:`E_B` is
the vector of eigenvalues. By convention the global ground-state energy is
subtracted at construction, so that the smallest eigenvalue across all
subspaces is zero. The invariant subspaces are built either by an
auto-partition procedure, by an explicit list of quantum-number operators, or
by a particle-number window, and each block is then diagonalized with the QR
algorithm.

**Notation used throughout the API:**

- :math:`B,\, B'` : subspace index (parameter ``sp_index``),
- :math:`\dim(B)` : dimension of subspace :math:`B`, returned by
  :func:`AtomDiagReal.get_subspace_dim`,
- :math:`E_{B,i}` : the :math:`i`-th eigenvalue inside subspace :math:`B`,
- :math:`N` : dimension of the full Hilbert space.

**Atomic Green's function.**

Derived from a solved diagonalization problem, the atomic Green's function is
built in the Lehmann (spectral) representation,

.. math::
   G_{ab}(z) = \frac{1}{Z} \sum_{B, B'}
     \sum_{i=0}^{\dim(B)-1} \sum_{j=0}^{\dim(B')-1}
     \Bigl( e^{-\beta E_{B,i}} + \eta\, e^{-\beta E_{B',j}} \Bigr)
     \frac{\langle B, i\,|\, \hat c_a \,|\, B', j\rangle\,
           \langle B', j\,|\, \hat c_b^\dagger \,|\, B, i\rangle}
          {z + E_{B,i} - E_{B',j}} \; ,

where :math:`\eta = +1` for fermionic and :math:`\eta = -1` for bosonic
statistics, :math:`Z` is the partition function at inverse temperature
:math:`\beta`, and the orbital indices :math:`a, b` run over one
Green's-function block (per ``gf_struct``). The only non-zero contributions
come from :math:`(B, B')` pairs that are connected by the application of a
creation/annihilation operator. The poles :math:`p = E_{B',j} - E_{B,i}` and
the corresponding residues are first stored in the Lehmann data structure,
then evaluated on the requested target mesh (imaginary time, Matsubara,
Legendre, or real frequency).

**What this module exposes.**

The solver itself is exposed through :class:`AtomDiagReal` and
:class:`AtomDiagComplex`; the dispatching factory :func:`AtomDiag` chooses the
appropriate variant based on the Hamiltonian (real-valued is the fast,
memory-efficient default; the complex variant is required when the Hamiltonian
has non-trivial phases such as spin-orbit coupling or a complex hybridization).
A set of free functions builds thermodynamic averages
(:func:`partition_function`, :func:`atomic_density_matrix`,
:func:`trace_rho_op`), applies operators on states (:func:`act`), tabulates
quantum-number eigenvalues (:func:`quantum_number_eigenvalues`,
:func:`quantum_number_eigenvalues_checked`), and assembles the atomic Green's
function on different meshes (:func:`atomic_g_tau`, :func:`atomic_g_iw`,
:func:`atomic_g_l`, :func:`atomic_g_w`) from a solved instance.
"""


from triqs.operators import Operator
from itertools import chain
from .atom_diag import AtomDiagReal, AtomDiagComplex
from .atom_diag import partition_function, atomic_density_matrix, trace_rho_op, act
from .atom_diag import quantum_number_eigenvalues, quantum_number_eigenvalues_checked
from .atom_diag import atomic_g_tau, atomic_g_iw, atomic_g_l, atomic_g_w

# Construct real/complex AtomDiag
def AtomDiag(*args, **kwargs):
    """Construct an exact diagonalization solver, dispatched on the Hamiltonian type.

    Returns :class:`AtomDiagReal` when the Hamiltonian (and any further operator
    arguments) is purely real, and :class:`AtomDiagComplex` otherwise. The
    arguments are forwarded unchanged to the chosen class constructor; see
    :class:`AtomDiagReal` for the full list of supported overloads.

    Parameters
    ----------
    h : Operator
        Many-body Hamiltonian to be diagonalized.
    *args, **kwargs
        Additional positional and keyword arguments forwarded to the
        :class:`AtomDiagReal` / :class:`AtomDiagComplex` constructor (e.g.
        ``fops``, ``hyb``, ``n_min``/``n_max`` or ``qn_vector``).

    Returns
    -------
    AtomDiagReal or AtomDiagComplex
        Solved diagonalization problem.
    """
    if len(args) > 0: h = args[0]
    elif 'h' in kwargs: h = kwargs['h']
    else:
        raise RuntimeError("You must provide a Hamiltonian!")
    if not isinstance(h, Operator):
        raise RuntimeError("The Hamiltonian must be an Operator object!")

    if all(x.imag.is_zero() for x in chain(args, kwargs.keys()) if isinstance(x, Operator)):
        op_to_real = lambda x: x.real if isinstance(x, Operator) else x
        args_r = [op_to_real(x) for x in args]
        kwargs_r = {k: op_to_real(v) for k,v in kwargs.items()}
        return AtomDiagReal(*args_r, **kwargs_r)

    return AtomDiagComplex(*args, **kwargs)

__all__ = ['AtomDiag','AtomDiagReal','AtomDiagComplex',
           'partition_function','atomic_density_matrix','trace_rho_op','act',
           'quantum_number_eigenvalues','quantum_number_eigenvalues_checked',
           'atomic_g_tau','atomic_g_iw','atomic_g_l','atomic_g_w']
