from pytriqs.gf import *
from pytriqs.plot.mpl_interface import *
from numpy import *
import os

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

        self.g0t << Fourier(self.g0)
        self.sigmat << (self.U**2) * self.g0t * self.g0t * self.g0t
        self.sigma << Fourier(self.sigmat)

        # Dyson equation to get G
        self.g << inverse(inverse(self.g0) - self.sigma)

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
    S.g << SemiCircular(2*t)

    # Do the DMFT loop
    for i in range(n_loops):
        S.g0 << inverse( iOmega_n - t**2 * S.g )
        S.solve()

    # Get the real-axis with Pade approximants
    greal = GfReFreq(indices = [1], window = (-4.0,4.0), n_points = 400)
    greal.set_from_pade(S.g, 201, 0.0)

    r=(U-Umin)/(Umax-Umin) #for color
    oplot((-1/pi*greal).imag, lw=3,RI='S', color=(r,1.-r,1.-r), label = "U=%1.1f"%U)
plt.xlim(-4,4)
plt.ylim(0,0.7)
plt.ylabel("$A(\omega)$");
