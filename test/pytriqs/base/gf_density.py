import numpy as np
from pytriqs.gf import *

def fermi(eps, beta):
    return 1./(1. + np.exp(beta * eps))

beta = 50.0

# -- Test Matsubara frequency density for free Gf

g_iw = GfImFreq(beta=beta, indices=[0], n_points=1000)
g_iw << inverse(iOmega_n)

np.testing.assert_almost_equal(g_iw.density(), 0.5)

np.random.seed(1337) # reproducible random values

for eps in np.random.random(10):
    g_iw << inverse(iOmega_n - eps)
    km = make_zero_tail(g_iw, 2)
    km[1] = np.eye(g_iw.target_shape[0])
    n = np.squeeze(g_iw.density(km))
    n_ref = fermi(eps, beta)
    np.testing.assert_almost_equal(n, n_ref)

# -- Real frequency
     
g_iw = GfImFreq(beta=beta, indices=[0, 1], n_points=1000)

# -- Test intervals with both odd and even discretizations
g_w_even = GfReFreq(indices=[0, 1], window=[-10., 11.], n_points=10000)
g_w_odd = GfReFreq(indices=[0, 1], window=[-10., 11.], n_points=10001)

def get_g(g_w):

    """ General 2x2 Green's function with continous spectral
    function and off diagonal elements. """
    
    delta_w = g_w.copy()

    delta_w[0,0] << -1.25 + 1.*SemiCircular(5.)
    delta_w[1,1] << +1.00 + 1.*SemiCircular(5.)
    
    delta_w[0,1] << 2.1*(0.05 + 0.05j) * SemiCircular(1.)
    delta_w[1,0] << 2.1*(0.05 - 0.05j) * SemiCircular(1.)

    g_w << inverse(Omega - delta_w)
    
    return g_w

g_iw = get_g(g_iw)
g_w_even = get_g(g_w_even)
g_w_odd = get_g(g_w_odd)

N_iw = g_iw.total_density()
n_iw = g_iw.density()

N_w_even = g_w_even.total_density(beta)
N_w_odd = g_w_odd.total_density(beta)

n_w_even = g_w_even.density(beta)
n_w0_even = g_w_even.density()

n_w_odd = g_w_odd.density(beta)
n_w0_odd = g_w_odd.density()

np.testing.assert_almost_equal(N_iw, N_w_even, decimal=5)
np.testing.assert_almost_equal(N_iw, N_w_odd, decimal=5)

np.testing.assert_almost_equal(n_iw, n_w_even, decimal=5)
np.testing.assert_almost_equal(n_iw, n_w_odd, decimal=5)

# -- Zero temperature result is only close to low temp result
np.testing.assert_almost_equal(n_iw, n_w0_even, decimal=3)
np.testing.assert_almost_equal(n_iw, n_w0_odd, decimal=3)

# -- Block gf total density

G1 = Gf(mesh=g_w_even.mesh, target_shape=(2,2))
G2 = G1.copy()

G1 << SemiCircular(2.)
G2 << SemiCircular(4.)

BG = BlockGf(name_list=['0', '1'], block_list=[G1, G2])

for block, g in [('0', G1), ('1', G2)]:
    np.testing.assert_array_almost_equal(BG.density()[block], g.density())
    np.testing.assert_array_almost_equal(BG.density(beta)[block], g.density(beta))

np.testing.assert_almost_equal(
    G1.total_density() + G2.total_density(),
    BG.total_density())

np.testing.assert_almost_equal(
    G1.total_density(beta) + G2.total_density(beta),
    BG.total_density(beta))
