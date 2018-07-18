""" Tail fit of the simplest Green's function imaginable

G(i\omega_n) = 1/(i\omega_n - 1)

which has the high frequency expansion

G(i\omega_n) = \sum_{k=1}^\infty 1/(i\omega_n)^k

i.e. its high frequency coefficients are equal to unity. """

import numpy as np

from pytriqs.gf import Gf, MeshImFreq, inverse, iOmega_n

def get_error_per_order(beta, n_w):

    mesh = MeshImFreq(beta, 'Fermion', n_w)
    G = Gf(mesh=mesh, target_shape=[1, 1])

    G << inverse( iOmega_n - 1.0 )
    tail, err = G.fit_tail()

    tail_ref = np.array([[ [0.] + [1.]*(tail.shape[0]-1) ]]).T

    tail_err = np.abs(tail_ref - tail)

    print 'beta =', beta
    print 'n_w =', n_w
    print 'error in coeffs ='
    print tail_err[:, :, 0]

    np.testing.assert_array_almost_equal(tail_err, np.zeros_like(tail_err))
    
# -----

n_w = 1000
beta_vec = [100., 10., 1., 0.1]

for beta in beta_vec:
    get_error_per_order(beta, n_w)

