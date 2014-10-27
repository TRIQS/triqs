from pytriqs.gf.local import *

class IPTSolver:

    def __init__(self, **params):

        self.U = params['U']
        self.beta = params['beta']

        # Matsubara frequency
        self.g_iw = GfImFreq(indices=[0], beta=self.beta)
        self.g0_iw = self.g_iw.copy()
        self.sigma_iw = self.g_iw.copy()

        # Imaginary time
        self.g0_tau = GfImTime(indices=[0], beta = self.beta)
        self.sigma_tau = self.g0_tau.copy()

    def solve(self):

        self.g0_tau <<= InverseFourier(self.g0_iw)
        self.sigma_tau <<= (self.U**2) * self.g0_tau * self.g0_tau * self.g0_tau
        self.sigma_iw <<= Fourier(self.sigma_tau)

        # Dyson equation to get G
        self.g_iw <<= inverse(inverse(self.g0_iw) - self.sigma_iw)
