from pytriqs.gf.local import *

class IPTSolver:

    def __init__(self, **params):

        self.U = params['U']
        self.beta = params['beta']

        # Matsubara frequency
        self.g = GfImFreq(indices=[0], beta=self.beta)
        self.g0 = self.g.copy()
        self.sigma = self.g.copy()

        # Imaginary time
        self.g0t = GfImTime(indices=[0], beta = self.beta)
        self.sigmat = self.g0t.copy()

    def solve(self):

        self.g0t <<= InverseFourier(self.g0)
        self.sigmat <<= (self.U**2) * self.g0t * self.g0t * self.g0t
        self.sigma <<= Fourier(self.sigmat)

        # Dyson equation to get G
        self.g <<= self.g0 * inverse(1.0 - self.sigma * self.g0)
