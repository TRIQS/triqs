# Copyright (c) 2021 Simons Foundation
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
# Authors: Nils Wentzell

import unittest

from triqs.operators import c, c_dag, Operator
from triqs.atom_diag import *
from itertools import product

import numpy as np

orbs = (1, 2, 3)
spins = ("dn", "up")

# Construct a sum of the pair hopping and spin flip terms from a 3-orbital Kanamori interaction Hamiltonian
H_p = Operator()
H_J = Operator()

for o1, o2 in product(orbs, repeat = 2):
    if o1 == o2: continue
    H_p += c_dag("dn", o1) * c_dag("up", o1) * c("up", o2) * c("dn", o2)
    H_J += c_dag("dn", o1) * c("up", o1) * c_dag("up", o2) * c("dn", o2)

H = H_p + H_J

fops1 = [(s, o) for (s, o) in product(spins, orbs)]
fops2 = [(s, o) for (o, s) in product(orbs, spins)]

ad1 = AtomDiag(H, fops1)
ad2 = AtomDiag(H, fops2)

# for e1, e2 in zip(ad1.energies, ad2.energies):
    # print(e1.round(5), e2.round(5))

# Check that subspace energies are identical up to ordering
assert len(ad1.energies) == len(ad2.energies)
assert all(sum(np.array_equal(es1, es) for es1 in ad1.energies) == sum(np.array_equal(es2, es) for es2 in ad2.energies) for es in ad1.energies)
