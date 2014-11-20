from pytriqs.gf.local import *
from pytriqs.plot.mpl_interface import *
from numpy import *
import os

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

        self.g0_tau << InverseFourier(self.g0_iw)
        self.sigma_tau << (self.U**2) * self.g0_tau * self.g0_tau * self.g0_tau
        self.sigma_iw << Fourier(self.sigma_tau)

        # Dyson equation to get G
        self.g_iw << inverse(inverse(self.g0_iw) - self.sigma_iw)


# Parameters
t = 0.5
beta = 40
n_loops = 20
dos_files = []

# Prepare the plot
plt.figure(figsize=(6,6))
plt.title("Local DOS, IPT, Bethe lattice, $\\beta=%.2f$"%(beta))

# Main loop over U
Umax=4.05
Umin=0.0
for U in arange(Umin, Umax, 0.51):

    # Construct the IPT solver and set initial G
    S = IPTSolver(U = U, beta = beta)
    S.g_iw << SemiCircular(2*t)

    # Do the DMFT loop
    for i in range(n_loops):
        S.g0_iw << inverse( iOmega_n - t**2 * S.g_iw )
        S.solve()

    # Get the real-axis with Pade approximants
    greal = GfReFreq(indices = [1], window = (-4.0,4.0), n_points = 400)
    greal.set_from_pade(S.g_iw, 201, 0.0)

    r=(U-Umin)/(Umax-Umin) #for color
    oplot((-1/pi*greal).imag, lw=3,RI='S', color=(r,1.-r,1.-r), label = "U=%1.1f"%U)
plt.xlim(-4,4)
plt.ylim(0,0.7)
plt.ylabel("$A(\omega)$");
