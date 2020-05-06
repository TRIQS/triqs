from triqs.gf import *
from triqs.plot.mpl_interface import oplot

# A Green's function on the Matsubara axis set to a semicircular
gw = GfImFreq(indices = [1], beta = 50)
gw << SemiCircular(half_bandwidth = 1)

# Create an imaginary-time Green's function
gt = GfImTime(indices = [1], beta = 50)
gt << Fourier(gw)

# Plot the Legendre Green's function
oplot(gt, '-')
