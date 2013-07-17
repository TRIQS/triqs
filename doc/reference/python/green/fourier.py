from pytriqs.gf.local import *
from pytriqs.plot.mpl_interface import oplot,plt

# A Green's function on the Matsubara axis set to a semicircular
gw = GfImFreq(indices = [1], beta = 50)
gw <<= SemiCircular(half_bandwidth = 1)

# Create an imaginary-time Green's function and plot it
gt = GfImTime(indices = [1], beta = 50)
gt <<= InverseFourier(gw)
oplot(gt, '-')
