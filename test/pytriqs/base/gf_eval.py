from pytriqs.gf.local import *
from numpy import *

def are_close(a,b):
  return abs(a-b)<1e-10

beta = 10.0
n = 2

g_iw = GfImFreq(beta=beta, indices=[0])
g_iw << iOmega_n

assert are_close(1j*(2*n+1)*pi/beta, g_iw(n)[0,0])

g_iw.invert()
g_tau = GfImTime(beta=beta, indices=[0])
g_tau << InverseFourier(g_iw)

assert are_close(g_tau(1.)[0,0], -0.5)
assert are_close(g_tau(11.)[0,0], 0.5)

g_w = GfReFreq(indices=[0], window=[0.,1.], n_points = 1000)
g_w << Omega+0.1

assert are_close(g_w(0.5)[0,0], 0.6)

g_w.invert()
g_t = GfReTime(indices=[0], window=[0.,2*pi*999/(1+1/999.)], n_points = 1000)
g_t << InverseFourier(g_w)

assert are_close(g_t(1.)[0,0], -0.926625205354-0.0852181350513j)

