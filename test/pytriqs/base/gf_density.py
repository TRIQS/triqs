import numpy as np
from pytriqs.gf.local import *

np.random.seed(1337)

def fermi(eps, beta):
    return 1./(1. + np.exp(-beta * eps))

beta = 10.0

g_iw = GfImFreq(beta=beta, indices=[0], n_points=1000)
g_iw << inverse(iOmega_n)
np.testing.assert_almost_equal(g_iw.density(), 0.5)

for eps in np.random.random(10):
    g_iw << inverse(iOmega_n + eps)
    n = np.squeeze(g_iw.density())
    n_ref = fermi(eps, beta)
    np.testing.assert_almost_equal(n, n_ref)

# -- Real frequency
     
D = 2.
eps = 1e-9

g_w_even = GfReFreq(indices=[0], window=[-2.+eps, 2.-eps], n_points=100000)
g_w_odd = GfReFreq(indices=[0], window=[-2.+eps, 2.-eps], n_points=100001)

g_iw << SemiCircular(D)
g_w_even << SemiCircular(D)
g_w_odd << SemiCircular(D)

n_iw = g_iw.density()

n_w_even = g_w_even.density(beta=beta)
n_w0_even = g_w_even.density(beta=-1.)
n_w_odd = g_w_odd.density(beta=beta)
n_w0_odd = g_w_odd.density(beta=-1.)

np.testing.assert_almost_equal(n_iw, n_w_even)
np.testing.assert_almost_equal(n_iw, n_w_odd)
np.testing.assert_almost_equal(n_iw, n_w0_even)
np.testing.assert_almost_equal(n_iw, n_w0_odd)

