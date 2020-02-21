from __future__ import print_function
from __future__ import unicode_literals
import numpy as np
from pytriqs.gf import *

def fermi(eps, beta):
    return 1./(1. + np.exp(beta * eps))

beta = 50.0

# -- Test Matsubara frequency density for free Gf

g_iw = GfImFreq(beta=beta, indices=[0], n_points=1000)

print("==============================================")

print(g_iw.density())

for eps in np.random.random(10):
    g_iw << inverse(iOmega_n - eps)
    km = make_zero_tail(g_iw, 2)
    km[1] = np.eye(g_iw.target_shape[0])
    n = np.squeeze(g_iw.density(km))
    n_ref = fermi(eps, beta)
    np.testing.assert_almost_equal(n, n_ref)
