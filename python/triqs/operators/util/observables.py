# Copyright (c) 2015-2017 Igor Krivenko
# Copyright (c) 2020 Simons Foundation
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

"""Operators for commonly used observables."""

import numpy as np
from triqs.operators.operators import Operator, n, c_dag, c
from triqs.operators.util.op_struct import get_mkind
from triqs.operators.util.U_matrix import spherical_to_cubic
from itertools import product

pauli_matrix = {'x' : np.array([[0,1],[1,0]]),
                'y' : np.array([[0,-1j],[1j,0]]),
                'z' : np.array([[1,0],[0,-1]]),
                '+' : np.array([[0,2],[0,0]]),
                '-' : np.array([[0,0],[2,0]])}

# Helper function for backward compat and improved error messages
def check_backward_compat(fname):
    if orb_names is not None:
        raise RuntimeError("Argument orb_names is no longer supported. Please provide n_orb instead.")
    if isinstance(n_orb, list):
        import warnings
        warnings.warn("Positional argument n_orb of function {} should provide the number of orbitals, not a list".format(fname))
        n_orb = len(n_orb)

def N_op(spin_names, n_orb, off_diag = None, map_operator_structure = None, orb_names = None):
    r"""
    Create an operator of the total number of particles.

    .. math:: \hat N = \sum_{i\sigma} a_{i\sigma}^\dagger a_{i\sigma}.

    Parameters
    ----------
    spin_names : list of strings
               Names of the spins, e.g. ['up','down'].
    n_orb : int
               Number of orbitals.
    off_diag : boolean
               Do we have (orbital) off-diagonal elements?
               If yes, the operators and blocks are denoted by ('spin', 'orbital'),
               otherwise by ('spin_orbital',0).
    map_operator_structure : dict
                             Mapping of names of GF blocks names from one convention to another,
                             e.g. {('up', 0): ('up_0', 0), ('down', 0): ('down_0',0)}.
                             If provided, the operators and blocks are denoted by the mapping of ``('spin', 'orbital')``.

    Returns
    -------
    N : Operator
        The total number of particles.

    """
    check_backward_compat("N_op")
    mkind = get_mkind(off_diag,map_operator_structure)
    N = Operator()
    for sn, o in product(spin_names,range(n_orb)): N += n(*mkind(sn,o))
    return N

def S_op(component, spin_names, n_orb, off_diag = None, map_operator_structure = None, orb_names = None):
    r"""
    Create a component of the spin vector operator.

    .. math::
        \hat S_{x,y,z} = \frac{1}{2}\sum_{i\sigma\sigma'} a^\dagger_{i\sigma} \mathbf{\tau}^{x,y,z}_{\sigma\sigma'} a_{i\sigma'},
        \quad\hat S_\pm = \hat S_x \pm i \hat S_y.

    Parameters
    ----------
    component : string
               Component to be created, one of 'x', 'y', 'z', '+', or '-'.
    spin_names : list of strings
               Names of the spins, e.g. ['up','down'].
    n_orb : int
               Number of orbitals.
    off_diag : boolean
               Do we have (orbital) off-diagonal elements?
               If yes, the operators and blocks are denoted by ('spin', 'orbital'),
               otherwise by ('spin_orbital',0).
    map_operator_structure : dict
                             Mapping of names of GF blocks names from one convention to another,
                             e.g. {('up', 0): ('up_0', 0), ('down', 0): ('down_0',0)}.
                             If provided, the operators and blocks are denoted by the mapping of ``('spin', 'orbital')``.

    Returns
    -------
    S : Operator
        The component of the spin vector operator.

    """
    check_backward_compat("S_op")
    mkind  = get_mkind(off_diag,map_operator_structure)
    pm = pauli_matrix[component]

    S = Operator()
    spin_range = list(range(len(spin_names)))
    for n1, n2 in product(spin_range,spin_range):
        for o in range(n_orb):
            S += 0.5 * c_dag(*mkind(spin_names[n1],o)) * pm[n1,n2] * c(*mkind(spin_names[n2],o))
    return S

def S2_op(spin_names, n_orb, off_diag = None, map_operator_structure = None, orb_names = None):
    r"""
    Create the square of the total spin operator.

    .. math:: \hat S^2 = \hat S_x^2 + \hat S_y^2 + \hat S_z^2.

    Parameters
    ----------
    spin_names : list of strings
               Names of the spins, e.g. ['up','down'].
    n_orb : int
               Number of orbitals.
    off_diag : boolean
               Do we have (orbital) off-diagonal elements?
               If yes, the operators and blocks are denoted by ('spin', 'orbital'),
               otherwise by ('spin_orbital',0).
    map_operator_structure : dict
                             Mapping of names of GF blocks names from one convention to another,
                             e.g. {('up', 0): ('up_0', 0), ('down', 0): ('down_0',0)}.
                             If provided, the operators and blocks are denoted by the mapping of ``('spin', 'orbital')``.

    Returns
    -------
    S2 : Operator
        The square of the total spin operator.

    """
    check_backward_compat("S2_op")
    Sz, Sp, Sm = [S_op(k,spin_names,n_orb,off_diag,map_operator_structure) for k in ('z','+','-')]
    return Sz*Sz + 0.5*(Sp*Sm + Sm*Sp)

def L_op(component, spin_names, n_orb, off_diag = None, map_operator_structure = None, basis='spherical', T=None, orb_names = None):
    r"""
    Create a component of the orbital momentum vector operator.

    .. math::
        \hat L_{z,+,-} &= \sum_{ii'\sigma} a^\dagger_{i\sigma} L^{z,+,-}_{ii'} a_{i'\sigma},\\
        \hat L_x &= \frac{1}{2}(\hat L_+ + \hat L_-),\ \hat L_y = \frac{1}{2i}(\hat L_+ - \hat L_-),\\
        L^z_{ii'} &= i\delta_{i,i'}, \
        L^+_{ii'} = \delta_{i,i'+1}\sqrt{l(l+1)-i'(i'+1)}, \
        L^+_{ii'} = \delta_{i,i'-1}\sqrt{l(l+1)-i'(i'-1)}.

    Parameters
    ----------
    component : string
               Component to be created, one of 'x', 'y', 'z', '+', or '-'.
    spin_names : list of strings
               Names of the spins, e.g. ['up','down'].
    n_orb : int
               Number of orbitals.
    off_diag : boolean
               Do we have (orbital) off-diagonal elements?
               If yes, the operators and blocks are denoted by ('spin', 'orbital'),
               otherwise by ('spin_orbital',0).
    map_operator_structure : dict
                             Mapping of names of GF blocks names from one convention to another,
                             e.g. {('up', 0): ('up_0', 0), ('down', 0): ('down_0',0)}.
                             If provided, the operators and blocks are denoted by the mapping of ``('spin', 'orbital')``.
    basis : string, optional
            The basis in which the interaction matrix should be computed.
            Takes the values

            - 'spherical': spherical harmonics,
            - 'cubic': cubic harmonics (valid only for the integer orbital momenta, i.e. for odd values of n_orb),
            - 'other': other basis type as given by the transformation matrix T.

    T : real/complex numpy array, optional
        Transformation matrix for basis change.
        Must be provided if basis='other'.

    Returns
    -------
    L : Operator
        The component of the orbital momentum vector operator.

    """
    check_backward_compat("L_op")
    l = (n_orb-1)/2.0
    L_melem_dict = {'z' : lambda m,mp: m if np.isclose(m,mp) else 0,
                    '+' : lambda m,mp: np.sqrt(l*(l+1)-mp*(mp+1)) if np.isclose(m,mp+1) else 0,
                    '-' : lambda m,mp: np.sqrt(l*(l+1)-mp*(mp-1)) if np.isclose(m,mp-1) else 0,
                    'x' : lambda m,mp: 0.5*(L_melem_dict['+'](m,mp) + L_melem_dict['-'](m,mp)),
                    'y' : lambda m,mp: -0.5j*(L_melem_dict['+'](m,mp) - L_melem_dict['-'](m,mp))}
    L_melem = L_melem_dict[component]
    orb_range = list(range(int(2*l+1)))
    L_matrix = np.array([[L_melem(o1-l,o2-l) for o2 in orb_range] for o1 in orb_range])

    # Transform from spherical basis if needed
    if basis == "cubic":
        if not np.isclose(np.mod(l,1),0):
            raise ValueError("L_op: cubic basis is only defined for the integer orbital momenta.")
        T = spherical_to_cubic(int(l))
    if basis == "other" and T is None: raise ValueError("L_op: provide T for other bases.")
    if T is not None: L_matrix = np.einsum("ij,jk,kl",np.conj(T),L_matrix,np.transpose(T))

    mkind = get_mkind(off_diag,map_operator_structure)
    L = Operator()
    for sn in spin_names:
        for o1, o2 in product(orb_range,orb_range):
            L += c_dag(*mkind(sn,o1)) * L_matrix[o1,o2] * c(*mkind(sn,o2))
    return L

def L2_op(spin_names, n_orb, off_diag = None, map_operator_structure = None, basis='spherical', T=None, orb_names = None):
    r"""
    Create the square of the orbital momentum operator.

    .. math:: \hat L^2 = \hat L_x^2 + \hat L_y^2 + \hat L_z^2.

    Parameters
    ----------
    spin_names : list of strings
               Names of the spins, e.g. ['up','down'].
    n_orb : int
               Number of orbitals.
    off_diag : boolean
               Do we have (orbital) off-diagonal elements?
               If yes, the operators and blocks are denoted by ('spin', 'orbital'),
               otherwise by ('spin_orbital',0).
    map_operator_structure : dict
                             Mapping of names of GF blocks names from one convention to another,
                             e.g. {('up', 0): ('up_0', 0), ('down', 0): ('down_0',0)}.
                             If provided, the operators and blocks are denoted by the mapping of ``('spin', 'orbital')``.
    basis : string, optional
            The basis in which the interaction matrix should be computed.
            Takes the values

            - 'spherical': spherical harmonics,
            - 'cubic': cubic harmonics (valid only for the integer orbital momenta, i.e. for odd values of n_orb),
            - 'other': other basis type as given by the transformation matrix T.

    T : real/complex numpy array, optional
        Transformation matrix for basis change.
        Must be provided if basis='other'.

    Returns
    -------
    L2 : Operator
        The square of the orbital momentum operator.
    """
    check_backward_compat("L2_op")
    Lz, Lp, Lm = [L_op(k,spin_names,n_orb,off_diag, map_operator_structure, basis, T) for k in ('z','+','-')]
    return Lz*Lz + 0.5*(Lp*Lm + Lm*Lp)

def LS_op(spin_names, n_orb, off_diag = None, map_operator_structure = None, basis='spherical', T=None, orb_names = None):
    r"""
    Create a spin-orbital coupling operator.

    .. math:: \mathbf{\hat L\cdot\hat S} = \hat L_x \hat S_x  + \hat L_y \hat S_y + \hat L_z \hat S_z.

    Parameters
    ----------
    spin_names : list of strings
               Names of the spins, e.g. ['up','down'].
    n_orb : int
               Number of orbitals.
    off_diag : boolean
               Do we have (orbital) off-diagonal elements?
               If yes, the operators and blocks are denoted by ('spin', 'orbital'),
               otherwise by ('spin_orbital',0).
    map_operator_structure : dict
                             Mapping of names of GF blocks names from one convention to another,
                             e.g. {('up', 0): ('up_0', 0), ('down', 0): ('down_0',0)}.
                             If provided, the operators and blocks are denoted by the mapping of ``('spin', 'orbital')``.
    basis : string, optional
            The basis in which the interaction matrix should be computed.
            Takes the values

            - 'spherical': spherical harmonics,
            - 'cubic': cubic harmonics (valid only for the integer orbital momenta, i.e. for odd values of n_orb),
            - 'other': other basis type as given by the transformation matrix T.

    T : real/complex numpy array, optional
        Transformation matrix for basis change.
        Must be provided if basis='other'.

    Returns
    -------
    LS : Operator
        The spin-orbital coupling operator.
    """
    check_backward_compat("LS_op")
    Sz, Sp, Sm = [S_op(k,spin_names,n_orb,off_diag,map_operator_structure) for k in ('z','+','-')]
    Lz, Lp, Lm = [L_op(k,spin_names,n_orb,off_diag,map_operator_structure, basis, T) for k in ('z','+','-')]
    return Lz*Sz + 0.5*(Lp*Sm + Lm*Sp)
