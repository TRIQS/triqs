from pytriqs.operators import *
from pytriqs.gf import *
from pytriqs.archive import *
from pytriqs.atom_diag import *
from pytriqs.utility.comparison_tests import *
import numpy as np
from itertools import product

save_h5 = False

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

ad_r = AtomDiag(make_hamiltonian(0.4, 1.0, 0.3, 0.03, 0.2), fops)
ad_c = AtomDiag(make_hamiltonian(0.4, 1.0, 0.3, 0.03, 0.2j), fops)

gf_struct = [['dn',[0,1,2]],['up',[0,1,2]]]
beta = 10

# GF (real)
G_tau = atomic_g_tau(ad_r, beta, gf_struct, 400)
G_iw = atomic_g_iw(ad_r, beta, gf_struct, 100)
G_l = atomic_g_l(ad_r, beta, gf_struct, 20)
G_w = atomic_g_w(ad_r, beta, gf_struct, (-2, 2), 400, 0.01)

if save_h5:
    with HDFArchive('atom_diag_python.h5','w') as ar:
        ar["real"] = {'G_tau':G_tau, 'G_iw':G_iw, 'G_l':G_l, 'G_w':G_w}
else:
    with HDFArchive('atom_diag_python.ref.h5','r') as ref_ar:
        assert_block_gfs_are_close(ref_ar["real"]["G_tau"], G_tau)
        assert_block_gfs_are_close(ref_ar["real"]["G_iw"], G_iw)
        assert_block_gfs_are_close(ref_ar["real"]["G_l"], G_l)
        assert_block_gfs_are_close(ref_ar["real"]["G_w"], G_w)

# GF (complex)
G_tau = atomic_g_tau(ad_c, beta, gf_struct, 400)
G_iw = atomic_g_iw(ad_c, beta, gf_struct, 100)
G_l = atomic_g_l(ad_c, beta, gf_struct, 20)
G_w = atomic_g_w(ad_c, beta, gf_struct, (-2, 2), 400, 0.01)
if save_h5:
    with HDFArchive('atom_diag_python.h5','a') as ar:
        ar["complex"] = {'G_tau':G_tau, 'G_iw':G_iw, 'G_l':G_l, 'G_w':G_w}
else:
    with HDFArchive('atom_diag_python.ref.h5','r') as ref_ar:
        assert_block_gfs_are_close(ref_ar["complex"]["G_tau"], G_tau)
        assert_block_gfs_are_close(ref_ar["complex"]["G_iw"], G_iw)
        assert_block_gfs_are_close(ref_ar["complex"]["G_l"], G_l)
        assert_block_gfs_are_close(ref_ar["complex"]["G_w"], G_w)

# HDF5
with HDFArchive('atom_diag.h5', 'w') as ar:
    ar['ad_r'] = ad_r
    ar['ad_c'] = ad_c

with HDFArchive('atom_diag.h5', 'r') as ar:
    ad_1 = ar['ad_r']
    ad_2 = ar['ad_c']

assert isinstance(ad_1, AtomDiagReal)
assert isinstance(ad_2, AtomDiagComplex)
