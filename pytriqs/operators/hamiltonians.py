from pytriqs.operators import *
from itertools import product

# Define commonly-used Hamiltonians here: Slater, Kanamori, density-density

def h_loc_slater(spin_names,orb_names,U_matrix,off_diag=None,map_operator_structure=None,H_dump=None):

    if H_dump:
        H_dump_file = open(H_dump,'w')
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
                H_dump_file.write(mkind(s1,a1)[0] + '\t')
                H_dump_file.write(mkind(s2,a2)[0] + '\t')
                H_dump_file.write(mkind(s2,a3)[0] + '\t')
                H_dump_file.write(mkind(s1,a4)[0] + '\t')
                H_dump_file.write(str(U_val.real) + '\n')

    return H

def h_loc_kanamori(spin_names,orb_names,U,Uprime,J_hund,off_diag=None,map_operator_structure=None,H_dump=None):

    if H_dump:
        H_dump_file = open(H_dump,'w')
    H = Operator()
    mkind = get_mkind(off_diag,map_operator_structure)

    # density terms:
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
                H_dump_file.write("Density-density terms" + '\n')
                H_dump_file.write(mkind(s1,a1)[0] + '\t')
                H_dump_file.write(mkind(s2,a2)[0] + '\t')
                H_dump_file.write(str(U_val) + '\n')

    # spin-flip terms:
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
                H_dump_file.write("Spin-flip terms" + '\n')
                H_dump_file.write(mkind(s1,a1)[0] + '\t')
                H_dump_file.write(mkind(s2,a2)[0] + '\t')
                H_dump_file.write(mkind(s2,a3)[0] + '\t')
                H_dump_file.write(mkind(s1,a4)[0] + '\t')
                H_dump_file.write(str(-J_hund) + '\n')

    # pair-hopping terms:
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
                H_dump_file.write("Pair-hopping terms" + '\n')
                H_dump_file.write(mkind(s1,a1)[0] + '\t')
                H_dump_file.write(mkind(s2,a2)[0] + '\t')
                H_dump_file.write(mkind(s2,a3)[0] + '\t')
                H_dump_file.write(mkind(s1,a4)[0] + '\t')
                H_dump_file.write(str(-J_hund) + '\n')

    return H

def h_loc_density(spin_names,orb_names,U,Uprime,off_diag=None,map_operator_structure=None,H_dump=None):

    if H_dump:
        H_dump_file = open(H_dump,'w')
    H = Operator()
    mkind = get_mkind(off_diag,map_operator_structure)
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
                H_dump_file.write(mkind(s1,a1)[0] + '\t')
                H_dump_file.write(mkind(s2,a2)[0] + '\t')
                H_dump_file.write(str(U_val) + '\n')

    return H

# Set function to make index for GF blocks given spin sn and orbital name on
def get_mkind(off_diag,map_operator_structure):

    if (off_diag is None) and (map_operator_structure is None):
        raise ValueError("hamiltonians: provide either off_diag or map_operator_structure.")

    if map_operator_structure is None:
        if off_diag:
            mkind = lambda sn, on: (sn, on)
        else:
            mkind = lambda sn, on: (sn+'_%s'%on, 0)
    else:
        mkind = lambda sn, on: map_operator_structure[(sn,on)]

    return mkind

# Set block structure of GF
def set_operator_structure(spin_names,orb_names,off_diag):
    gf_struct = {}
    if off_diag: # outer blocks are spin blocks
        for sn in spin_names:
            gf_struct[sn] = [on for on in orb_names]
    else: # outer blocks are spin-orbital blocks
        for sn, on in product(spin_names,orb_names):
            gf_struct[sn+'_%s'%on] = [0]

    return gf_struct
