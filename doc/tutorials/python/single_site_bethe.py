from pytriqs.gf.local import *
from pytriqs.archive import *
import pytriqs.utility.mpi as mpi

# Set up a few parameters
Half_Bandwidth = 1.0    
U = 2.5
Chemical_Potential = U/2.0
Beta = 100
N_loops = 5

# Construct a CTQMC solver
from pytriqs.applications.impurity_solvers.operators import *                          # imports the class manipulating C, C_dagger and N = C_dagger C
from pytriqs.applications.impurity_solvers.cthyb_matrix import Solver        # imports the solver class
S = Solver(Beta = Beta,                                                       # inverse temperature
           GFstruct = [ ('up',[1]), ('down',[1]) ],                           # Structure of the Green function 
           H_Local = U * N('up',1) * N('down',1),                             # Local Hamiltonian 
           Quantum_Numbers = { 'Nup' : N('up',1), 'Ndown' : N('down',1) },    # Quantum Numbers (operators commuting with H_Local)
           N_Cycles = 5000,                                                   # Number of QMC cycles
           Length_Cycle = 200,                                                # Length of a cycle
           N_Warmup_Cycles = 1000,                                            # How many warmup cycles
           N_Legendre_Coeffs = 30,                                            # Use 30 Legendre coefficients to represent G(tau)
           Random_Generator_Name = "mt19937",                                 # Use the Mersenne Twister 19937 random generator
           Use_Segment_Picture = True)                                        # Here we can use the segment picture

# Initalize the Green's function to a semi circular
S.G <<= SemiCircular(Half_Bandwidth)

# Now do the DMFT loop 
for IterationNumber in range(N_loops):

    # Compute S.G0 with the self-consistency condition while imposing paramagnetism 
    g = 0.5 * ( S.G['up'] + S.G['down'] )
    for name, g0block in S.G0:
        g0block <<= inverse( iOmega_n + Chemical_Potential - (Half_Bandwidth/2.0)**2  * g )
    
    # Run the solver
    S.Solve()

    # Some intermediate saves
    if mpi.is_master_node():
      R = HDFArchive("single_site_bethe.h5")
      R["G-%s"%IterationNumber] = S.G
      del R

    # Here we would usually write some convergence test
    # if Converged : break  

