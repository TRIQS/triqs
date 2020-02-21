"""Operators for commonly used observables."""
from __future__ import unicode_literals

from builtins import range
import numpy as np
from pytriqs.operators.operators import Operator, n, c_dag, c
from pytriqs.operators.util.op_struct import get_mkind
from pytriqs.operators.util.U_matrix import spherical_to_cubic
from itertools import product

pauli_matrix = {'x' : np.array([[0,1],[1,0]]),
                'y' : np.array([[0,-1j],[1j,0]]),
                'z' : np.array([[1,0],[0,-1]]),
                '+' : np.array([[0,2],[0,0]]),
                '-' : np.array([[0,0],[2,0]])}

def N_op(spin_names, orb_names, off_diag = None, map_operator_structure = None):
    r"""
    Create an operator of the total number of particles.

    .. math:: \hat N = \sum_{i\sigma} a_{i\sigma}^\dagger a_{i\sigma}.

    Parameters
    ----------
    spin_names : list of strings
                 Names of the spins, e.g. ['up','down'].
    orb_names : list of strings or int
                Names of the orbitals, e.g. [0,1,2] or ['t2g','eg'].
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
    mkind = get_mkind(off_diag,map_operator_structure)
    N = Operator()
    for sn, on in product(spin_names,orb_names): N += n(*mkind(sn,on))
    return N

def S_op(component, spin_names, orb_names, off_diag = None, map_operator_structure = None):
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
    orb_names : list of strings or int
                Names of the orbitals, e.g. [0,1,2] or ['t2g','eg'].
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
    mkind  = get_mkind(off_diag,map_operator_structure)
    pm = pauli_matrix[component]

    S = Operator()
    spin_range = list(range(len(spin_names)))
    for n1, n2 in product(spin_range,spin_range):
        for on in orb_names:
            S += 0.5 * c_dag(*mkind(spin_names[n1],on)) * pm[n1,n2] * c(*mkind(spin_names[n2],on))
    return S

def S2_op(spin_names, orb_names, off_diag = None, map_operator_structure = None):
    r"""
    Create the square of the total spin operator.

    .. math:: \hat S^2 = \hat S_x^2 + \hat S_y^2 + \hat S_z^2.

    Parameters
    ----------
    spin_names : list of strings
                 Names of the spins, e.g. ['up','down'].
    orb_names : list of strings or int
                Names of the orbitals, e.g. [0,1,2] or ['t2g','eg'].
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
    Sz, Sp, Sm = [S_op(k,spin_names,orb_names,off_diag,map_operator_structure) for k in ('z','+','-')]
    return Sz*Sz + 0.5*(Sp*Sm + Sm*Sp)

def L_op(component, spin_names, orb_names, off_diag = None, map_operator_structure = None, basis='spherical', T=None):
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
    orb_names : list of strings or int
                Names of the orbitals, e.g. [0,1,2] or ['t2g','eg'].
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
            - 'cubic': cubic harmonics (valid only for the integer orbital momenta, i.e. for odd sizes of orb_names),
            - 'other': other basis type as given by the transformation matrix T.

    T : real/complex numpy array, optional
        Transformation matrix for basis change.
        Must be provided if basis='other'.

    Returns
    -------
    L : Operator
        The component of the orbital momentum vector operator.

    """
    l = (len(orb_names)-1)/2.0
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
            L += c_dag(*mkind(sn,orb_names[o1])) * L_matrix[o1,o2] * c(*mkind(sn,orb_names[o2]))
    return L

def L2_op(spin_names, orb_names, off_diag = None, map_operator_structure = None, basis='spherical', T=None):
    r"""
    Create the square of the orbital momentum operator.

    .. math:: \hat L^2 = \hat L_x^2 + \hat L_y^2 + \hat L_z^2.

    Parameters
    ----------
    spin_names : list of strings
                 Names of the spins, e.g. ['up','down'].
    orb_names : list of strings or int
                Names of the orbitals, e.g. [0,1,2] or ['t2g','eg'].
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
            - 'cubic': cubic harmonics (valid only for the integer orbital momenta, i.e. for odd sizes of orb_names),
            - 'other': other basis type as given by the transformation matrix T.

    T : real/complex numpy array, optional
        Transformation matrix for basis change.
        Must be provided if basis='other'.

    Returns
    -------
    L2 : Operator
        The square of the orbital momentum operator.
    """
    Lz, Lp, Lm = [L_op(k,spin_names,orb_names,off_diag, map_operator_structure, basis, T) for k in ('z','+','-')]
    return Lz*Lz + 0.5*(Lp*Lm + Lm*Lp)

def LS_op(spin_names, orb_names, off_diag = None, map_operator_structure = None, basis='spherical', T=None):
    r"""
    Create a spin-orbital coupling operator.

    .. math:: \mathbf{\hat L\cdot\hat S} = \hat L_x \hat S_x  + \hat L_y \hat S_y + \hat L_z \hat S_z.

    Parameters
    ----------
    spin_names : list of strings
                 Names of the spins, e.g. ['up','down'].
    orb_names : list of strings or int
                Names of the orbitals, e.g. [0,1,2] or ['t2g','eg'].
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
            - 'cubic': cubic harmonics (valid only for the integer orbital momenta, i.e. for odd sizes of orb_names),
            - 'other': other basis type as given by the transformation matrix T.

    T : real/complex numpy array, optional
        Transformation matrix for basis change.
        Must be provided if basis='other'.

    Returns
    -------
    LS : Operator
        The spin-orbital coupling operator.
    """
    Sz, Sp, Sm = [S_op(k,spin_names,orb_names,off_diag,map_operator_structure) for k in ('z','+','-')]
    Lz, Lp, Lm = [L_op(k,spin_names,orb_names,off_diag,map_operator_structure, basis, T) for k in ('z','+','-')]
    return Lz*Sz + 0.5*(Lp*Sm + Lm*Sp)
