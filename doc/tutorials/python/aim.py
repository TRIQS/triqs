from pytriqs.gf.local import *
from pytriqs.operators import *
from pytriqs.applications.impurity_solvers.cthyb_matrix import Solver

D, V, U = 1.0, 0.2, 4.0
e_f, Beta = -U/2.0, 50

# The impurity solver
S = Solver(Beta = Beta,                             # inverse temperature
           GFstruct = [ ('up',[1]), ('down',[1]) ], # Structure of the Green's function 
           H_Local = U * N('up',1) * N('down',1),   # Local Hamiltonian 
           Quantum_Numbers = {                      # Quantum Numbers 
               'Nup' : N('up',1),                   # (operators commuting with H_Local) 
               'Ndown' : N('down',1) },          
           N_Cycles  = 500000,                      # Number of QMC cycles
           Length_Cycle = 200,                      # Length of one cycle 
           N_Warmup_Cycles = 10000,                 # Warmup cycles
           N_Legendre_Coeffs = 50,                  # Number of Legendre coefficients
           Random_Generator_Name = 'mt19937',       # Name of the random number generator
           Use_Segment_Picture = True,              # Use the segment picture
           Measured_Operators = {                   # Operators to be averaged
             'Nimp' : N('up',1)+N('down',1) },
           Global_Moves = [                         # Global move in the QMC
               (0.05, lambda (a,alpha,dag) : ( {'up':'down','down':'up'}[a],alpha,dag ) ) ], 
           )

# Initialize the non-interacting Green's function S.G0
for spin, g0 in S.G0 :
    g0 <<= inverse( iOmega_n - e_f - V**2 * Wilson(D) ) 

# Run the solver. The result will be in S.G
S.Solve()

# Save the results in an hdf5 file (only on the master node)
from pytriqs.archive import HDFArchive
import pytriqs.utility.mpi as mpi

if mpi.is_master_node():
  Results = HDFArchive("solution.h5",'w')
  Results["G"] = S.G
  Results["Gl"] = S.G_Legendre
  Results["Nimp"] = S.Measured_Operators_Results['Nimp']
