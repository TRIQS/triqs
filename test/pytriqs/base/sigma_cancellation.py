""" Regression test and playgound for the preservation of second order moment
in G0_iw in a DMFT lattice self consistency.

G = 1/N \sum_k [ i\omega_n - e_k - Sigma ]

\Delta = G^{-1} - i\omega_n + Sigma

Look at how the 0th order tail coeff of \Delta
deviates from the analytic result 

t_0 = 1/N \sum_k e_k 

Author: Hugo U.R. Strand 

"""
import matplotlib.pyplot as plt
##%matplotlib inline

import numpy as np
from pytriqs.gf import Gf, MeshImFreq, iOmega_n, inverse

def delta_inv(beta, nw, nk=100):

    mesh = MeshImFreq(beta, 'Fermion', n_max=nw)

    Sigma0, Sigma1 = 1.337, 3.5235
    ek = 2.*np.random.random(nk) - 1.

    G = Gf(mesh=mesh, target_shape=[1, 1])
    Sig = G.copy()
    for w in Sig.mesh:
        Sig[w][:] = Sigma0 + Sigma1 /w
 
    for e in ek: G << G + inverse(iOmega_n - e - Sig)

    G /= nk

    Delta = G.copy()
    Delta << inverse(G) - iOmega_n + Sig

    sum_ek = - np.sum(ek) / nk

    Roo = np.abs(Sigma0) + 1.

    w = [ w for w in mesh ]
    wmax = np.abs(w[-1].value)

    tail, err = Delta.fit_tail()

    order = len(tail)

    trunc_err_anal = (Roo / (0.8 * wmax))**order

    ratio = tail[0] / sum_ek
    diff = np.abs(1 - ratio)

    print '-' * 72
    print 'beta =', beta
    print 'nw =', nw
    print 'wmax =', wmax
    print 'tail_fit order =', len(tail)
    print 'tail_fit err =', err
    print tail[:3]
    #print sum_ek
    print 'trunc_err_anal =', trunc_err_anal
    print 'ratio =', ratio
    print 'diff =', diff

    return diff[0, 0]

beta = 500.0
nw = 1000
diff = delta_inv(beta, nw)

assert( diff < 2e-6 )

if True:

    beta_vec = np.array([1., 10., 100., 1000.])
    diff_vec = np.zeros_like(beta_vec)

    for idx, beta in enumerate(beta_vec):
        diff_vec[idx] = delta_inv(beta, nw)
        
    plt.plot(beta_vec, diff_vec, '-o')
    plt.semilogy([], [])
    plt.show()
