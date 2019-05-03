from pytriqs.gf import *
from pytriqs.operators import *
from pytriqs.operators.util.hamiltonians import h_int_kanamori
from pytriqs.archive import HDFArchive
from pytriqs.atom_diag import *
import numpy as np
from itertools import product

# Definition of a 3-orbital atom
spin_names = ('up','dn')
orb_names = [0,1,2]
# Set of fundamental operators
fops = [(sn,on) for sn, on in product(spin_names,orb_names)]

# Numbers of particles with spin up/down
N_up = n('up',0) + n('up',1) + n('up',2)
N_dn = n('dn',0) + n('dn',1) + n('dn',2)

# Construct Hubbard-Kanamori Hamiltonian
U = 3.0 * np.ones((3,3))
Uprime = 2.0 * np.ones((3,3))
J_hund = 0.5

H = h_int_kanamori(spin_names, orb_names, U, Uprime, J_hund, True)

# Add chemical potential
H += -4.0 * (N_up + N_dn)

# Add hopping terms between orbitals 0 and 1 with some complex amplitude
H += 0.1j * (c_dag('up',0) * c('up',1) - c_dag('up',1) * c('up',0))
H += 0.1j * (c_dag('dn',0) * c('dn',1) - c_dag('dn',1) * c('dn',0))

# Split H into blocks and diagonalize it using N_up and N_dn quantum numbers
ad = AtomDiag(H, fops, [N_up,N_dn])
print ad.n_subspaces # Number of invariant subspaces, 4 * 4 = 16

# Now split using the total number of particles, N = N_up + N_dn
ad = AtomDiag(H, fops, [N_up+N_dn])
print ad.n_subspaces # 7

# Split the Hilbert space automatically
ad = AtomDiag(H, fops)
print ad.n_subspaces # 28

# Partition function for inverse temperature \beta=3
beta = 3
print partition_function(ad, beta)

# Equilibrium density matrix
dm = atomic_density_matrix(ad, beta)

# Expectation values of orbital double occupancies
print trace_rho_op(dm, n('up',0) * n('dn',0), ad)
print trace_rho_op(dm, n('up',1) * n('dn',1), ad)
print trace_rho_op(dm, n('up',2) * n('dn',2), ad)

# Atomic Green's functions
gf_struct = [['dn',orb_names],['up',orb_names]]
G_w = atomic_g_w(ad, beta, gf_struct, (-2, 2), 400, 0.01)
G_tau = atomic_g_tau(ad, beta, gf_struct, 400)
G_iw = atomic_g_iw(ad, beta, gf_struct, 100)
G_l = atomic_g_l(ad, beta, gf_struct, 20)

# Finally, we save our AtomDiag object for later use
with HDFArchive('atom_diag_example.h5') as ar:
    ar['ad'] = ad
