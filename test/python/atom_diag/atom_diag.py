# Copyright (c) 2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2017 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2019-2020 Simons Foundation
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
# Authors: Igor Krivenko, Nils Wentzell

from triqs.operators import *
from triqs.gf import *
from h5 import *
from triqs.atom_diag import *
from triqs.utility.comparison_tests import *
import numpy as np
from itertools import product

save_h5 = False

fops = [(s,o) for s,o in product(('up','dn'),list(range(3)))]

def make_hamiltonian(mu, U, J, b, t):
    h = Operator()
    orbs = list(range(3))
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

gf_struct = [('dn',3), ('up',3)]
beta = 10

# GF (real)
G_tau = atomic_g_tau(ad_r, beta, gf_struct, 400)
G_iw = atomic_g_iw(ad_r, beta, gf_struct, 100)
G_l = atomic_g_l(ad_r, beta, gf_struct, 20)
G_w = atomic_g_w(ad_r, beta, gf_struct, (-2, 2), 400, 0.01)

if save_h5:
    with HDFArchive('atom_diag.h5','w') as ar:
        ar["real"] = {'G_tau':G_tau, 'G_iw':G_iw, 'G_l':G_l, 'G_w':G_w}
else:
    with HDFArchive('atom_diag.ref.h5','r') as ref_ar:
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
    with HDFArchive('atom_diag.h5','a') as ar:
        ar["complex"] = {'G_tau':G_tau, 'G_iw':G_iw, 'G_l':G_l, 'G_w':G_w}
else:
    with HDFArchive('atom_diag.ref.h5','r') as ref_ar:
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
