from pytriqs.operators import *
from pytriqs.gf import *
from pytriqs.archive import *
from pytriqs.atom_diag import *
from pytriqs.utility.comparison_tests import *
import numpy as np
from itertools import product
import pytriqs.utility.mpi as mpi


fops = [(s,o) for s,o in product(('up','dn'),range(3))]

def make_hamiltonian(mu, U, J, b, t):
    h = Operator()
    orbs = range(3)
    for o in orbs:
        h += sum(-mu * (n("up", o) + n("dn", o)) for o in orbs)
    for o in orbs:
        h += b * (n("up", o) - n("dn", o))
    for o in orbs:
        h += U * n("up", o) * n("dn", o)
    for o1, o2 in product(orbs,orbs):
        if o1 == o2: continue
        h += (U - 2 * J) * n("up", o1) * n("dn", o2)
    for o1, o2 in product(orbs,orbs):
        if o2 >= o1: continue
        h += (U - 3 * J) * n("up", o1) * n("up", o2)
        h += (U - 3 * J) * n("dn", o1) * n("dn", o2)
    for o1, o2 in product(orbs,orbs):
        if o1 == o2: continue
        h += -J * c_dag("up", o1) * c_dag("dn", o1) * c("up", o2) * c("dn", o2)
        h += -J * c_dag("up", o1) * c_dag("dn", o2) * c("up", o2) * c("dn", o1)

    h += t * c_dag("up", 0) * c("up", 1) + dagger(t * c_dag("up", 0) * c("up", 1))
    h += t * c_dag("dn", 0) * c("dn", 1) + dagger(t * c_dag("dn", 0) * c("dn", 1))

    return h

ad = AtomDiag(make_hamiltonian(0.4, 1.0, 0.3, 0.03, 0.2), fops)

gf_struct = [['dn',[0,1,2]],['up',[0,1,2]]]
beta = 10

# create GF for comparison
G_tau = atomic_g_tau(ad, beta, gf_struct, 400)
G_iw = atomic_g_iw(ad, beta, gf_struct, 100)
G_l = atomic_g_l(ad, beta, gf_struct, 20)
G_w = atomic_g_w(ad, beta, gf_struct, (-2, 2), 400, 0.01)

# HDF5
if mpi.is_master_node():
    with HDFArchive('atom_diag_bcast.h5', 'w') as ar:
        ar['ad'] = ad


ad_bcast = None
if mpi.is_master_node():
    with HDFArchive('atom_diag_bcast.h5', 'r') as ar:
        ad_bcast = ar['ad']

ad_bcast = mpi.bcast(ad_bcast)

assert isinstance(ad_bcast, AtomDiagReal)

# GF (real)
G_tau_bcast = atomic_g_tau(ad_bcast, beta, gf_struct, 400)
G_iw_bcast = atomic_g_iw(ad_bcast, beta, gf_struct, 100)
G_l_bcast = atomic_g_l(ad_bcast, beta, gf_struct, 20)
G_w_bcast = atomic_g_w(ad_bcast, beta, gf_struct, (-2, 2), 400, 0.01)

assert_block_gfs_are_close(G_tau_bcast, G_tau)
assert_block_gfs_are_close(G_iw_bcast, G_iw)
assert_block_gfs_are_close(G_l_bcast, G_l)
assert_block_gfs_are_close(G_w_bcast, G_w)

