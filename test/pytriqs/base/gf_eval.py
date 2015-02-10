from pytriqs.gf.local import *
from numpy import *

beta = 10.0
n = 2

g_iw = GfImFreq(beta=beta, indices=[0])
g_iw << iOmega_n

print 1j*(2*n+1)*pi/beta, "==", g_iw(n)[0,0]

g_iw.invert()
g_tau = GfImTime(beta=beta, indices=[0])
g_tau << InverseFourier(g_iw)

print "-0.5 == ", g_tau(1.)[0,0], "0.5 == ", g_tau(11.)[0,0]

g_w = GfReFreq(indices=[0], window=[0.,1.], n_points = 1000)
g_w << Omega+0.1

print 0.6, "==", g_w(0.5)[0,0]

g_w.invert()
g_t = GfReTime(indices=[0], window=[0.,2*pi*999/(1+1/999.)], n_points = 1000)
g_t << InverseFourier(g_w)

print g_t(1.)[0,0]

