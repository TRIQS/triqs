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
assert all(all(es1 == es2) for es1, es2 in zip(ad1.energies, ad2.energies))
