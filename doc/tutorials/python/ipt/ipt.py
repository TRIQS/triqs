import numpy

from pytriqs.gf.local import *

class Solver:
    """A simple IPT solver for the symmetric one band Anderson model"""

    def __init__(self, **params):
        self.name = 'Iterated Perturbation Theory'

        self.U = params['U']
        self.beta = params['beta']

        # Only one block in GFs
        g = GfImFreq(indices =[0], beta =self.beta, name ='0')
        self.G = BlockGf(name_list=('0',), block_list=(g,))
        self.G0 = self.G.copy()

    def Solve(self):

        # Imaginary time representation of G_0
        g0t = GfImTime(indices =[0], beta =self.beta, name ='0')
        G0t = BlockGf(name_list=('0',), block_list=(g0t,))
        G0t['0'].set_from_inverse_fourier(self.G0['0'])

        # IPT expression for the self-energy (particle-holy symmetric case is implied)
        Sigmat = G0t.copy()
        Sigmat['0'] <<= (self.U**2)*G0t['0']*G0t['0']*G0t['0']
        
        self.Sigma = self.G0.copy()
        self.Sigma['0'].set_from_fourier(Sigmat['0'])

        # New impurity GF from the Dyson's equation
        self.G <<= self.G0*inverse(1.0 - self.Sigma*self.G0)

S = 0

def run(**params):
    """IPT loop"""

    # Read input parameters
    U = params['U']
    beta = params['beta']
    N_loops = params['N_loops']
    Initial_G0 = params['Initial_G0']
    Self_Consistency = params['Self_Consistency']

    global S
    # Create a new IPT solver object
    S = Solver(U=U, beta=beta)
    # Initialize the bare GF using the function passed in through Initial_G0 parameter
    Initial_G0(S.G0)

    # DMFT iterations
    for IterationNumber in range(N_loops):
        print "DMFT iteration number ", IterationNumber

        S.Solve()
        Self_Consistency(S.G0,S.G)
