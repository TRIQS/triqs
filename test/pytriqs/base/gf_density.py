import numpy as np
from pytriqs.gf.local import *

np.random.seed(1337)

def fermi(eps, beta):
    return 1./(1. + np.exp(-beta * eps))

beta = 10.0

g_iw = GfImFreq(beta=beta, indices=[0])
g_iw << inverse(iOmega_n)
np.testing.assert_almost_equal(g_iw.density(), 0.5)

for eps in np.random.random(10):
    g_iw << inverse(iOmega_n + eps)
    n = np.squeeze(g_iw.density())
    n_ref = fermi(eps, beta)
    np.testing.assert_almost_equal(n, n_ref)

# -- Real frequency
     
g_w = GfReFreq(indices=[0], window=[-2., 2.])

D, mu = 2., .5
g_w << inverse(Omega - SemiCircular(D) + mu)
g_iw << inverse(iOmega_n - SemiCircular(D) + mu)

np.testing.assert_almost_equal(g_iw.density(), g_w.density(beta=beta))

w = np.array([w.real for w in g_w.mesh])
g = np.squeeze(g_w.data.imag)
norm = -1./np.pi * np.trapz(g, x=w)

np.testing.assert_almost_equal(norm, 1.)

