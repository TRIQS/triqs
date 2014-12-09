from pytriqs.gf.local import *
from pytriqs.plot.mpl_interface import oplot

# A Green's function on the Matsubara axis set to a semicircular
gw = GfImFreq(indices = [1], beta = 50)
gw << SemiCircular(half_bandwidth = 1)

# Create a Legendre Green's function with 40 coefficients
# initialize it from gw and plot it
gl = GfLegendre(indices = [1], beta = 50, n_points = 40)
gl << MatsubaraToLegendre(gw)
oplot(gl, '-o')
