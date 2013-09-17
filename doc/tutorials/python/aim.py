from pytriqs.gf.local import *
from pytriqs.operators import *
from pytriqs.applications.impurity_solvers.cthyb_matrix import Solver

# Parameters
D, V, U = 1.0, 0.2, 4.0
e_f, beta = -U/2.0, 50

# Construct the impurity solver with the inverse temperature
# and the structure of the Green's functions
S = Solver(beta = beta, gf_struct = [ ('up',[1]), ('down',[1]) ])

# Initialize the non-interacting Green's function S.G0
for spin, g0 in S.G0 :
    g0 <<= inverse( iOmega_n - e_f - V**2 * Wilson(D) ) 

# Run the solver. The result will be in S.G
S.solve(H_local = U * N('up',1) * N('down',1),   # Local Hamiltonian 
        quantum_numbers = {                      # Quantum Numbers 
          'Nup' : N('up',1),                     # Operators commuting with H_Local
          'Ndown' : N('down',1) },          
        n_cycles  = 500000,                      # Number of QMC cycles
        length_cycle = 200,                      # Length of one cycle 
        n_warmup_cycles = 10000,                 # Warmup cycles
        n_legendre = 50,                         # Number of Legendre coefficients
        random_name = 'mt19937',                 # Name of the random number generator
        use_segment_picture = True,              # Use the segment picture
        measured_operators = {                   # Operators to be averaged
          'Nimp' : N('up',1)+N('down',1) }
        )

# Save the results in an hdf5 file (only on the master node)
from pytriqs.archive import HDFArchive
import pytriqs.utility.mpi as mpi

if mpi.is_master_node():
  Results = HDFArchive("solution.h5",'w')
  Results["G"] = S.G
  Results["Gl"] = S.G_legendre
  Results["Nimp"] = S.measured_operators_results['Nimp']

