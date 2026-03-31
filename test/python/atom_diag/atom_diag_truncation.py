# Copyright (c) 2026 Simons Foundation
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
#     https://www.gnu.org/licenses/gpl-3.0.txt
#
# Authors: Nils Wentzell

# Test and demonstrate atom_diag.truncate().
#
# truncate(energy_cutoff, max_states) returns a new atom_diag retaining only
# the lowest-energy eigenstates. All derived quantities (c/cdag matrices,
# connections, vacuum state) are consistently updated, so the truncated object
# can be used directly in atomic_g_iw / atomic_g_tau.

from triqs.operators import *
from triqs.gfs import *
from h5 import HDFArchive
from triqs.atom_diag import *
import numpy as np
from itertools import product
import os

# ================================================================
# Hamiltonians
# ================================================================

def make_simple_hamiltonian():
    """2-orbital Hubbard atom with spin-flip hopping (4 Fock states)."""
    h = 2.0 * (n("up") - 0.5) * (n("dn") - 0.5)
    h += 0.3 * (c_dag("up") * c("dn") + c_dag("dn") * c("up"))
    return h

def make_kanamori_hamiltonian(mu, U, J, b, t):
    """3-orbital Kanamori atom with hopping between orbitals 0 and 1.

    Parameters: mu = chemical potential, U = intra-orbital interaction,
    J = Hund's coupling, b = magnetic field, t = hopping.
    Full Hilbert space: 2^6 = 64 states.
    """
    h = Operator()
    orbs = list(range(3))
    for o in orbs:
        h += -mu * (n("up", o) + n("dn", o))
        h += b * (n("up", o) - n("dn", o))
        h += U * n("up", o) * n("dn", o)
    for o1, o2 in product(orbs, orbs):
        if o1 == o2: continue
        h += (U - 2 * J) * n("up", o1) * n("dn", o2)
    for o1, o2 in product(orbs, orbs):
        if o2 >= o1: continue
        h += (U - 3 * J) * n("up", o1) * n("up", o2)
        h += (U - 3 * J) * n("dn", o1) * n("dn", o2)
    for o1, o2 in product(orbs, orbs):
        if o1 == o2: continue
        h += -J * c_dag("up", o1) * c_dag("dn", o1) * c("up", o2) * c("dn", o2)
        h += -J * c_dag("up", o1) * c_dag("dn", o2) * c("up", o2) * c("dn", o1)
    h += t * c_dag("up", 0) * c("up", 1) + dagger(t * c_dag("up", 0) * c("up", 1))
    h += t * c_dag("dn", 0) * c("dn", 1) + dagger(t * c_dag("dn", 0) * c("dn", 1))
    return h

# ================================================================
# 1. Small example: see exactly what truncation does
# ================================================================
print("=" * 60)
print("1. Small 2-orbital atom: energy cutoff truncation")
print("=" * 60)

h = make_simple_hamiltonian()
fops_simple = [("up",), ("dn",)]
ad_full = AtomDiag(h, fops_simple)

all_energies = sorted([e for sp_en in ad_full.energies for e in sp_en])
print(f"   Full Hilbert space:  {ad_full.full_hilbert_space_dim} states")
print(f"   Subspaces:           {ad_full.n_subspaces}")
print(f"   Eigenvalues (GS=0):  {[round(float(e), 4) for e in all_energies]}")

# Cut just below the highest eigenvalue
cutoff = all_energies[-1] - 0.1
ad_trunc = ad_full.truncate(energy_cutoff=cutoff)

trunc_energies = sorted([e for sp_en in ad_trunc.energies for e in sp_en])
print(f"   After truncate(energy_cutoff={cutoff:.2f}):")
print(f"     Kept {ad_trunc.total_eigenstate_count}/{ad_full.total_eigenstate_count} states")
print(f"     Eigenvalues:       {[round(float(e), 4) for e in trunc_energies]}")

assert not ad_full.is_truncated
assert ad_trunc.is_truncated
assert ad_trunc.total_eigenstate_count < ad_full.total_eigenstate_count
for sp in range(ad_trunc.n_subspaces):
    for i in range(ad_trunc.get_subspace_dim(sp)):
        assert ad_trunc.get_eigenvalue(sp, i) <= cutoff + 1e-10

# ================================================================
# 2. Larger atom: truncation by max_states
# ================================================================
print()
print("=" * 60)
print("2. 3-orbital Kanamori atom: max_states truncation")
print("=" * 60)

fops_kanamori = [(s, o) for s, o in product(('up', 'dn'), list(range(3)))]
h_kanamori = make_kanamori_hamiltonian(mu=0.4, U=1.0, J=0.3, b=0.03, t=0.2)
ad_full = AtomDiag(h_kanamori, fops_kanamori)

full_energies = sorted([e for sp_en in ad_full.energies for e in sp_en])
print(f"   Full Hilbert space:  {ad_full.full_hilbert_space_dim} states, {ad_full.n_subspaces} subspaces")
print(f"   Energy range:        [0, {full_energies[-1]:.2f}]")

max_states = 10
ad_trunc = ad_full.truncate(max_states=max_states)
trunc_energies = sorted([e for sp_en in ad_trunc.energies for e in sp_en])

print(f"   After truncate(max_states={max_states}):")
print(f"     Kept {ad_trunc.total_eigenstate_count} states in {ad_trunc.n_subspaces} subspaces")
print(f"     Energy range:      [0, {trunc_energies[-1]:.2f}]")

assert ad_trunc.total_eigenstate_count == max_states
# The kept energies must be the lowest max_states of the full spectrum
for i in range(max_states):
    assert abs(full_energies[i] - trunc_energies[i]) < 1e-10

# ================================================================
# 3. Combined criteria (energy cutoff AND max_states)
# ================================================================
print()
print("=" * 60)
print("3. Combined criteria: both energy_cutoff and max_states")
print("=" * 60)

energy_cutoff = 2.0
max_states = 20
ad_trunc = ad_full.truncate(energy_cutoff=energy_cutoff, max_states=max_states)

print(f"   truncate(energy_cutoff={energy_cutoff}, max_states={max_states}):")
print(f"     Kept {ad_trunc.total_eigenstate_count} states (limit: {max_states})")

assert ad_trunc.is_truncated
assert ad_trunc.total_eigenstate_count <= max_states
for sp in range(ad_trunc.n_subspaces):
    for i in range(ad_trunc.get_subspace_dim(sp)):
        assert ad_trunc.get_eigenvalue(sp, i) <= energy_cutoff + 1e-10

# ================================================================
# 4. Green's function from truncated atom_diag
# ================================================================
print()
print("=" * 60)
print("4. Atomic Green's function: full vs truncated")
print("=" * 60)

beta = 20.0
gf_struct = [('dn', 3), ('up', 3)]

G_iw_full = atomic_g_iw(ad_full, beta, gf_struct, 50)

# At inverse temperature beta, a state at energy E has Boltzmann weight
# ~ exp(-beta * E). Truncation error decreases as we include more states.
print(f"   beta = {beta}, full spectrum has {ad_full.total_eigenstate_count} states")
for cutoff in [1.0, 2.0, 3.0]:
    ad_t = ad_full.truncate(energy_cutoff=cutoff)
    G_iw_t = atomic_g_iw(ad_t, beta, gf_struct, 50)
    diff = max(np.max(np.abs(G_iw_full[bl].data - G_iw_t[bl].data)) for bl in ['up', 'dn'])
    print(f"   cutoff={cutoff:.1f}: {ad_t.total_eigenstate_count:2d} states, max|dG_iw| = {diff:.2e}")

# At cutoff=3.0, states at E=3 have weight exp(-60) ~ 0, so the GF is exact
ad_trunc = ad_full.truncate(energy_cutoff=3.0)
G_iw_trunc = atomic_g_iw(ad_trunc, beta, gf_struct, 50)
max_diff = max(np.max(np.abs(G_iw_full[bl].data - G_iw_trunc[bl].data)) for bl in ['up', 'dn'])
assert max_diff < 1e-8

# ================================================================
# 5. Vacuum state behavior under truncation
# ================================================================
print()
print("=" * 60)
print("5. Vacuum state: large mu pushes |0> above the energy cutoff")
print("=" * 60)

# At large mu, filling is energetically favored: the empty state |0> has
# bare energy ~ 0 while filled states have large negative bare energy.
# After the ground-state shift, |0> sits at a high eigenvalue.
h_large_mu = make_kanamori_hamiltonian(mu=5.0, U=1.0, J=0.3, b=0.03, t=0.2)
ad_full_mu = AtomDiag(h_large_mu, fops_kanamori)

all_en = sorted([e for sp_en in ad_full_mu.energies for e in sp_en])
print(f"   mu = 5.0: energy range [0, {all_en[-1]:.2f}]")
print(f"   Vacuum present in full: {ad_full_mu.has_vacuum}")

ad_trunc_mu = ad_full_mu.truncate(energy_cutoff=5.0)
print(f"   After truncate(energy_cutoff=5.0):")
print(f"     Kept {ad_trunc_mu.total_eigenstate_count}/{ad_full_mu.total_eigenstate_count} states")
print(f"     Vacuum present: {ad_trunc_mu.has_vacuum}")

assert ad_full_mu.has_vacuum
assert not ad_trunc_mu.has_vacuum
assert ad_trunc_mu.vacuum_subspace_index == -1

# At mu = 0, |0> is near the ground state and survives truncation.
h_zero_mu = make_kanamori_hamiltonian(mu=0.0, U=1.0, J=0.3, b=0.03, t=0.2)
ad_zero_mu = AtomDiag(h_zero_mu, fops_kanamori)

ad_trunc_zero = ad_zero_mu.truncate(energy_cutoff=2.0)
print(f"   mu = 0.0, truncate(energy_cutoff=2.0): vacuum present = {ad_trunc_zero.has_vacuum}")
assert ad_trunc_zero.has_vacuum

# ================================================================
# 6. Edge cases
# ================================================================
print()
print("=" * 60)
print("6. Edge cases: no-op truncation, idempotence, HDF5 roundtrip")
print("=" * 60)

# Default parameters -> no truncation
ad_copy = ad_full.truncate()
assert not ad_copy.is_truncated
assert ad_full.n_subspaces == ad_copy.n_subspaces
assert ad_full.total_eigenstate_count == ad_copy.total_eigenstate_count
print("   Default params (no truncation): OK")

# Idempotence: truncating twice with the same parameters gives the same result
ad_t1 = ad_full.truncate(energy_cutoff=2.0, max_states=20)
ad_t2 = ad_t1.truncate(energy_cutoff=2.0, max_states=20)
assert ad_t1.n_subspaces == ad_t2.n_subspaces
assert ad_t1.total_eigenstate_count == ad_t2.total_eigenstate_count
print("   Idempotence: OK")

# HDF5 roundtrip
ad_trunc = ad_full.truncate(energy_cutoff=2.0, max_states=15)
with HDFArchive('atom_diag_truncation_test.h5', 'w') as ar:
    ar['ad_trunc'] = ad_trunc
with HDFArchive('atom_diag_truncation_test.h5', 'r') as ar:
    ad_loaded = ar['ad_trunc']

assert ad_loaded.is_truncated == ad_trunc.is_truncated
assert ad_loaded.n_subspaces == ad_trunc.n_subspaces
assert ad_loaded.total_eigenstate_count == ad_trunc.total_eigenstate_count
print("   HDF5 roundtrip: OK")

os.remove('atom_diag_truncation_test.h5')

print()
print("All tests passed!")
