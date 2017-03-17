from pytriqs.operators import *
from pytriqs.gf.local import *
from pytriqs.archive import *
from pytriqs.atom_diag import *
from pytriqs.utility.comparison_tests import *
import numpy as np
from itertools import product

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

gf_struct = {'dn':[0,1,2],'up':[0,1,2]}
beta = 10

#G_tau_gen = lambda bn: (bn,GfImTime(beta=beta,indices=gf_struct[bn],n_points=400))
#G_iw_gen = lambda bn: (bn,GfImFreq(beta=beta,indices=gf_struct[bn],n_points=100))
#G_l_gen = lambda bn: (bn,GfLegendre(beta=beta,indices=gf_struct[bn],n_points=20))

#G_tau = BlockGf(name_block_generator = map(G_tau_gen, gf_struct))
#G_iw = BlockGf(name_block_generator = map(G_iw_gen, gf_struct))
#G_l = BlockGf(name_block_generator = map(G_l_gen, gf_struct))

## GF (real)
#agf = atomic_gf(ad_r, beta, gf_struct)
#G_tau << agf
#G_iw << agf
#G_l << agf
#with HDFArchive('atom_diag_python.ref.h5','r') as ar:
    ##ar["real"] = {'G_tau':G_tau,'G_iw':G_iw,'G_l':G_l}
    #assert_block_gfs_are_close(ar["real"]["G_tau"],G_tau)
    #assert_block_gfs_are_close(ar["real"]["G_iw"],G_iw)
    #assert_block_gfs_are_close(ar["real"]["G_l"],G_l)

## GF (complex)
#agf = atomic_gf(ad_c, beta, gf_struct)
#G_tau << agf
#G_iw << agf
#G_l << agf
#with HDFArchive('atom_diag_python.ref.h5','r') as ar:
    ##ar["complex"] = {'G_tau':G_tau,'G_iw':G_iw,'G_l':G_l}
    #assert_block_gfs_are_close(ar["complex"]["G_tau"],G_tau)
    #assert_block_gfs_are_close(ar["complex"]["G_iw"],G_iw)
    #assert_block_gfs_are_close(ar["complex"]["G_l"],G_l)

# HDF5
with HDFArchive('atom_diag.h5','w') as ar:
    ar['ad_r'] = ad_r
    ar['ad_c'] = ad_c

with HDFArchive('atom_diag.h5','r') as ar:
    ad_1 = ar['ad_r']
    ad_2 = ar['ad_c']

assert isinstance(ad_1,AtomDiagReal) and isinstance(ad_2,AtomDiagComplex)
