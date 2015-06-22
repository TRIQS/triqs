from pytriqs.operators import *
from op_struct import *
from itertools import product

# Define commonly-used Hamiltonians here: Slater, Kanamori, density-density

def h_int_slater(spin_names,orb_names,U_matrix,off_diag=None,map_operator_structure=None,H_dump=None):
    r"""
    Create a Slater Hamiltonian using fully rotationally-invariant 4-index interactions:

    .. math:: H = \sum_{ijkl,\sigma \sigma'} U_{ijkl} a_{i \sigma}^\dagger a_{j \sigma'}^\dagger a_{l \sigma'} a_{k \sigma}.

    Parameters
    ----------
    spin_names : list of strings
                 Names of the spins, e.g. ['up','down'].
    orb_names : list of strings or int       
                Names of the orbitals, e.g. [0,1,2] or ['t2g','eg'].
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

    Returns
    -------
    H : Operator
        The Hamiltonian.

    """

    if H_dump:
        H_dump_file = open(H_dump,'w')
        H_dump_file.write("Slater Hamiltonian:" + '\n')

    H = Operator()
    mkind = get_mkind(off_diag,map_operator_structure)
    for s1, s2 in product(spin_names,spin_names):
        for a1, a2, a3, a4 in product(orb_names,orb_names,orb_names,orb_names):
            U_val = U_matrix[orb_names.index(a1),orb_names.index(a2),orb_names.index(a3),orb_names.index(a4)]
            if abs(U_val.imag) > 1e-10:
                raise RuntimeError("Matrix elements of U are not real. Are you using a cubic basis?")

            H_term = 0.5 * U_val.real * c_dag(*mkind(s1,a1)) * c_dag(*mkind(s2,a2)) * c(*mkind(s2,a4)) * c(*mkind(s1,a3))
            H += H_term

            # Dump terms of H
            if H_dump and not H_term.is_zero():
                H_dump_file.write('%s'%(mkind(s1,a1),) + '\t')
                H_dump_file.write('%s'%(mkind(s2,a2),) + '\t')
                H_dump_file.write('%s'%(mkind(s2,a3),) + '\t')
                H_dump_file.write('%s'%(mkind(s1,a4),) + '\t')
                H_dump_file.write(str(U_val.real) + '\n')

    return H

def h_int_kanamori(spin_names,orb_names,U,Uprime,J_hund,off_diag=None,map_operator_structure=None,H_dump=None):
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
    orb_names : list of strings or int       
                Names of the orbitals, e.g. [0,1,2] or ['t2g','eg'].
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

    if H_dump:
        H_dump_file = open(H_dump,'w')
        H_dump_file.write("Kanamori Hamiltonian:" + '\n')

    H = Operator()
    mkind = get_mkind(off_diag,map_operator_structure)

    # density terms:
    if H_dump: H_dump_file.write("Density-density terms:" + '\n')
    for s1, s2 in product(spin_names,spin_names):
        for a1, a2 in product(orb_names,orb_names):
            if (s1==s2):
                U_val = U[orb_names.index(a1),orb_names.index(a2)]
            else:
                U_val = Uprime[orb_names.index(a1),orb_names.index(a2)]

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
        for a1, a2 in product(orb_names,orb_names):
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
        for a1, a2 in product(orb_names,orb_names):
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

def h_int_density(spin_names,orb_names,U,Uprime,off_diag=None,map_operator_structure=None,H_dump=None):
    r"""
    Create a density-density Hamiltonian.

    .. math::
        H = \frac{1}{2} \sum_{(i \sigma) \neq (j \sigma')} U_{i j}^{\sigma \sigma'} n_{i \sigma} n_{j \sigma'}.

    Parameters
    ----------
    spin_names : list of strings
                 Names of the spins, e.g. ['up','down'].
    orb_names : list of strings or int       
                Names of the orbitals, e.g. [0,1,2] or ['t2g','eg'].
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

    if H_dump:
        H_dump_file = open(H_dump,'w')
        H_dump_file.write("Density-density Hamiltonian:" + '\n')

    H = Operator()
    mkind = get_mkind(off_diag,map_operator_structure)
    if H_dump: H_dump_file.write("Density-density terms:" + '\n')
    for s1, s2 in product(spin_names,spin_names):
        for a1, a2 in product(orb_names,orb_names):
            if (s1==s2):
                U_val = U[orb_names.index(a1),orb_names.index(a2)]
            else:
                U_val = Uprime[orb_names.index(a1),orb_names.index(a2)]

            H_term = 0.5 * U_val * n(*mkind(s1,a1)) * n(*mkind(s2,a2))
            H += H_term

            # Dump terms of H
            if H_dump and not H_term.is_zero():
                H_dump_file.write('%s'%(mkind(s1,a1),) + '\t')
                H_dump_file.write('%s'%(mkind(s2,a2),) + '\t')
                H_dump_file.write(str(U_val) + '\n')

    return H
