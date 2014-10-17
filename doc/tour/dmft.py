from pytriqs.gf.local import *
from pytriqs.operators import *
from pytriqs.archive import *
import pytriqs.utility.mpi as mpi

# Set up a few parameters
U = 2.5
half_bandwidth = 1.0
chemical_potential = U/2.0
beta = 100
n_loops = 5

# Construct the CTQMC solver
from pytriqs.applications.impurity_solvers.cthyb_matrix import Solver
S = Solver(beta = beta, gf_struct = [ ('up',[1]), ('down',[1]) ])

# Initalize the Green's function to a semi circular
S.G <<= SemiCircular(half_bandwidth)

# Now do the DMFT loop
for IterationNumber in range(n_loops):

    # Compute S.G0 with the self-consistency condition while imposing paramagnetism
    g = 0.5 * ( S.G['up'] + S.G['down'] )
    for name, g0block in S.G0:
        g0block <<= inverse( iOmega_n + chemical_potential - (half_bandwidth/2.0)**2  * g )

    # Run the solver
    S.solve(H_local = U * N('up',1) * N('down',1),                          # Local Hamiltonian
            quantum_numbers = { 'Nup' : N('up',1), 'Ndown' : N('down',1) }, # Quantum Numbers (operators commuting with H_Local)
            n_cycles = 5000,                                                # Number of QMC cycles
            length_cycle = 200,                                             # Length of a cycle
            n_warmup_cycles = 1000,                                         # How many warmup cycles
            n_legendre = 30,                                                # Use 30 Legendre coefficients to represent G(tau)
            random_name = "mt19937",                                        # Use the Mersenne Twister 19937 random generator
            use_segment_picture = True)                                     # Here we can use the segment picture

    # Some intermediate saves
    if mpi.is_master_node():
      R = HDFArchive("single_site_bethe.h5")
      R["G-%s"%IterationNumber] = S.G
      del R

    # Here we would usually write some convergence test
    # if Converged : break
