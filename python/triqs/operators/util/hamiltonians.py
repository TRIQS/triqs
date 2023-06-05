# Copyright (c) 2014-2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2014-2016 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020-2022 Simons Foundation
# Copyright (c) 2015-2016 Igor Krivenko
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
# Authors: Michel Ferrero, Gernot Kraberger, Igor Krivenko, Priyanka Seth, Nils Wentzell

import operator
from triqs.operators import *
from .op_struct import *
from itertools import product
from functools import reduce

# Helper function for backward compat and improved error messages
def backward_compat(fname,n_orb,orb_names):
    if orb_names is not None:
        raise RuntimeError("Argument orb_names is no longer supported. Please provide n_orb instead.")
    if isinstance(n_orb, list):
        import warnings
        warnings.warn("{} takes as a second argument the number of orbitals, not a list of orbital names".format(fname))
        n_orb = len(n_orb)
    return n_orb

# Define commonly-used Hamiltonians here: Slater, Kanamori, density-density

def h_int_slater(spin_names,n_orb,U_matrix,off_diag=None,map_operator_structure=None,H_dump=None,complex=False,orb_names=None):
    r"""
    Create a Slater Hamiltonian using fully rotationally-invariant 4-index interactions:

    .. math:: H = \frac{1}{2} \sum_{ijkl,\sigma \sigma'} U_{ijkl} a_{i \sigma}^\dagger a_{j \sigma'}^\dagger a_{l \sigma'} a_{k \sigma}.

    Parameters
    ----------
    spin_names : list of strings
               Names of the spins, e.g. ['up','down'].
    n_orb : int
               Number of orbitals.
    U_matrix : 4D matrix or array
               The fully rotationally-invariant 4-index interaction :math:`U_{ijkl}`.
    off_diag : boolean
               Do we have (orbital) off-diagonal elements?
               If yes, the operators and blocks are denoted by ('spin', 'orbital'),
               otherwise by ('spin_orbital',0).
    map_operator_structure : dict
                             Mapping of names of GF blocks names from one convention to another,
                             e.g. {('up', 0): ('up_0', 0), ('down', 0): ('down_0',0)}.
                             If provided, the operators and blocks are denoted by the mapping of ``('spin', 'orbital')``.
    H_dump : string
             Name of the file to which the Hamiltonian should be written.
    complex : bool
             Whether there are complex values in the interaction. If False, passing a complex U will
             cause an error.

    Returns
    -------
    H : Operator
        The Hamiltonian.

    """
    n_orb = backward_compat("h_int_slater",n_orb,orb_names)

    if H_dump:
        H_dump_file = open(H_dump,'w')
        H_dump_file.write("Slater Hamiltonian:" + '\n')

    H = Operator()
    mkind = get_mkind(off_diag,map_operator_structure)
    for s1, s2 in product(spin_names,spin_names):
        for a1, a2, a3, a4 in product(range(n_orb), range(n_orb), range(n_orb), range(n_orb)):
            U_val = U_matrix[a1,a2,a3,a4]
            if abs(U_val.imag) > 1e-10 and not complex:
                raise RuntimeError("Matrix elements of U are not real. Are you using a cubic basis?")

            H_term = 0.5 * (U_val if complex else U_val.real) * c_dag(*mkind(s1,a1)) * c_dag(*mkind(s2,a2)) * c(*mkind(s2,a4)) * c(*mkind(s1,a3))
            H += H_term

            # Dump terms of H
            if H_dump and not H_term.is_zero():
                H_dump_file.write('%s'%(mkind(s1,a1),) + '\t')
                H_dump_file.write('%s'%(mkind(s2,a2),) + '\t')
                H_dump_file.write('%s'%(mkind(s2,a3),) + '\t')
                H_dump_file.write('%s'%(mkind(s1,a4),) + '\t')
                H_dump_file.write(str(U_val.real) + '\n')

    return H

def h_int_kanamori(spin_names,n_orb,U,Uprime,J_hund,off_diag=None,map_operator_structure=None,H_dump=None,orb_names=None):
    r"""
    Create a Kanamori Hamiltonian using the density-density, spin-fip and pair-hopping interactions.

    .. math::
        H = \frac{1}{2} \sum_{(i \sigma) \neq (j \sigma')} U_{i j}^{\sigma \sigma'} n_{i \sigma} n_{j \sigma'}
            - \sum_{i \neq j} J a^\dagger_{i \uparrow} a_{i \downarrow} a^\dagger_{j \downarrow} a_{j \uparrow}
            + \sum_{i \neq j} J a^\dagger_{i \uparrow} a^\dagger_{i \downarrow} a_{j \downarrow} a_{j \uparrow}.

    Parameters
    ----------
    spin_names : list of strings
               Names of the spins, e.g. ['up','down'].
    n_orb : int
               Number of orbitals.
    U : 2D matrix or array
               :math:`U_{ij}^{\sigma \sigma} (same spins)`
    Uprime : 2D matrix or array
               :math:`U_{ij}^{\sigma \bar{\sigma}} (opposite spins)`
    J_hund : scalar
               :math:`J`
    off_diag : boolean
               Do we have (orbital) off-diagonal elements?
               If yes, the operators and blocks are denoted by ('spin', 'orbital'),
               otherwise by ('spin_orbital',0).
    map_operator_structure : dict
               Mapping of names of GF blocks names from one convention to another,
               e.g. {('up', 0): ('up_0', 0), ('down', 0): ('down_0',0)}.
               If provided, the operators and blocks are denoted by the mapping of ``('spin', 'orbital')``.
    H_dump : string
               Name of the file to which the Hamiltonian should be written.

    Returns
    -------
    H : Operator
        The Hamiltonian.

    """
    n_orb = backward_compat("h_int_kanamori",n_orb,orb_names)

    if H_dump:
        H_dump_file = open(H_dump,'w')
        H_dump_file.write("Kanamori Hamiltonian:" + '\n')

    H = Operator()
    mkind = get_mkind(off_diag,map_operator_structure)

    # density terms:
    if H_dump: H_dump_file.write("Density-density terms:" + '\n')
    for s1, s2 in product(spin_names,spin_names):
        for a1, a2 in product(range(n_orb), range(n_orb)):
            if (s1==s2):
                U_val = U[a1,a2]
            else:
                U_val = Uprime[a1,a2]

            H_term = 0.5 * U_val * n(*mkind(s1,a1)) * n(*mkind(s2,a2))
            H += H_term

            # Dump terms of H
            if H_dump and not H_term.is_zero():
                H_dump_file.write('%s'%(mkind(s1,a1),) + '\t')
                H_dump_file.write('%s'%(mkind(s2,a2),) + '\t')
                H_dump_file.write(str(U_val) + '\n')

    # spin-flip terms:
    if H_dump: H_dump_file.write("Spin-flip terms:" + '\n')
    for s1, s2 in product(spin_names,spin_names):
        if (s1==s2):
            continue
        for a1, a2 in product(range(n_orb), range(n_orb)):
            if (a1==a2):
                continue
            H_term = -0.5 * J_hund * c_dag(*mkind(s1,a1)) * c(*mkind(s2,a1)) * c_dag(*mkind(s2,a2)) * c(*mkind(s1,a2))
            H += H_term

            # Dump terms of H
            if H_dump and not H_term.is_zero():
                H_dump_file.write('%s'%(mkind(s1,a1),) + '\t')
                H_dump_file.write('%s'%(mkind(s2,a2),) + '\t')
                H_dump_file.write(str(-J_hund) + '\n')

    # pair-hopping terms:
    if H_dump: H_dump_file.write("Pair-hopping terms:" + '\n')
    for s1, s2 in product(spin_names,spin_names):
        if (s1==s2):
            continue
        for a1, a2 in product(range(n_orb), range(n_orb)):
            if (a1==a2):
                continue
            H_term = 0.5 * J_hund * c_dag(*mkind(s1,a1)) * c_dag(*mkind(s2,a1)) * c(*mkind(s2,a2)) * c(*mkind(s1,a2))
            H += H_term

            # Dump terms of H
            if H_dump and not H_term.is_zero():
                H_dump_file.write('%s'%(mkind(s1,a1),) + '\t')
                H_dump_file.write('%s'%(mkind(s2,a2),) + '\t')
                H_dump_file.write(str(-J_hund) + '\n')

    return H

def h_int_density(spin_names,n_orb,U,Uprime,off_diag=None,map_operator_structure=None,H_dump=None,orb_names=None):
    r"""
    Create a density-density Hamiltonian.

    .. math::
        H = \frac{1}{2} \sum_{(i \sigma) \neq (j \sigma')} U_{i j}^{\sigma \sigma'} n_{i \sigma} n_{j \sigma'}.

    Parameters
    ----------
    spin_names : list of strings
               Names of the spins, e.g. ['up','down'].
    n_orb : int
               Number of orbitals.
    U : 2D matrix or array
               :math:`U_{ij}^{\sigma \sigma} (same spins)`
    Uprime : 2D matrix or array
               :math:`U_{ij}^{\sigma \bar{\sigma}} (opposite spins)`
    off_diag : boolean
               Do we have (orbital) off-diagonal elements?
               If yes, the operators and blocks are denoted by ('spin', 'orbital'),
               otherwise by ('spin_orbital',0).
    map_operator_structure : dict
               Mapping of names of GF blocks names from one convention to another,
               e.g. {('up', 0): ('up_0', 0), ('down', 0): ('down_0',0)}.
               If provided, the operators and blocks are denoted by the mapping of ``('spin', 'orbital')``.
    H_dump : string
               Name of the file to which the Hamiltonian should be written.

    Returns
    -------
    H : Operator
        The Hamiltonian.

    """
    n_orb = backward_compat("h_int_density",n_orb,orb_names)

    if H_dump:
        H_dump_file = open(H_dump,'w')
        H_dump_file.write("Density-density Hamiltonian:" + '\n')

    H = Operator()
    mkind = get_mkind(off_diag,map_operator_structure)
    if H_dump: H_dump_file.write("Density-density terms:" + '\n')
    for s1, s2 in product(spin_names,spin_names):
        for a1, a2 in product(range(n_orb),range(n_orb)):
            if (s1==s2):
                U_val = U[a1,a2]
            else:
                U_val = Uprime[a1,a2]

            H_term = 0.5 * U_val * n(*mkind(s1,a1)) * n(*mkind(s2,a2))
            H += H_term

            # Dump terms of H
            if H_dump and not H_term.is_zero():
                H_dump_file.write('%s'%(mkind(s1,a1),) + '\t')
                H_dump_file.write('%s'%(mkind(s2,a2),) + '\t')
                H_dump_file.write(str(U_val) + '\n')

    return H

def diagonal_part(H):
    r"""
    Extract the density part from an operator H.

    The density part is a sum of all those monomials of H that are
    products of occupation number operators :math:`n_1 n_2 n_3 \ldots`.

    Parameters
    ----------
    H : Operator
        The operator from which the density part is extracted.

    Returns
    -------
    n_part : Operator
             The density part of H.
    """
    n_part = Operator()
    for indices, coeff in H:
        c_ind, c_dag_ind = set(), set()
        for dag, ind in indices:
            (c_dag_ind if dag else c_ind).add(tuple(ind))
        if c_ind == c_dag_ind: # This monomial is of n-type
            n_part += coeff * reduce(operator.mul,
                              [c_dag(*dag_ind[1]) if dag_ind[0] else c(*dag_ind[1]) for dag_ind in indices],
                              Operator(1))
    return n_part

def make_operator_real(H, tol = 0):
    r"""
    Return the real part of a given operator H checking that its
    imaginary part is below tolerance.

    Parameters
    ----------
    H : Operator
        The operator to be converted.

    tol : float
          Tolerance threshold for the imaginary part of the operator's coefficients.

    Returns
    -------
    H_real : Operator
             The real part of H.
    """
    if any(abs(term[-1].imag) > tol for term in H):
        raise RuntimeError("A coefficient of the operator has an imaginary part above tolerance")
    return H.real
