""" Bracket accessor/slicer tests for gf_multivar

Author: Hugo U.R. Strand (2017) hugo.strand@gmail.com

Whish list:

1. Mesh value type and operator() arg are not the same

for tau in g.mesh:
   print g(tau)

should be possible, right now we have to do

for tau in g.mesh:
   print g(tau.real)

2. Linear interpolation for higher order tau greens function

g4_tau(t1, t2, t3)

"""

import itertools
import numpy as np

from pytriqs.gf import Gf
from pytriqs.gf import GfImTime
from pytriqs.gf import MeshImTime, MeshProduct

ntau = 10
beta = 1.2345

g_tau = GfImTime(name='g_tau', beta=beta,
                 statistic='Fermion', n_points=ntau,
                 indices=[1])

tau = np.array([tau for tau in g_tau.mesh])
g_ref = np.exp(-beta * tau)
g_tau.data[:, 0, 0] = np.exp(-beta * tau)

for idx, tau in enumerate(g_tau.mesh):

    # comparison does not work at beta since the evaluation g_tau() wraps..
    if idx == len(g_tau.mesh)-1: break
    
    #diff_interp = g_tau(tau)[0,0] - g_ref[idx] # FIXME: tau is complex
    
    diff_interp = g_tau(tau.real)[0,0] - g_ref[idx]
    diff_dbrack = g_tau[[idx]][0,0] - g_ref[idx]

    np.testing.assert_almost_equal(diff_interp, 0.0)
    np.testing.assert_almost_equal(diff_dbrack, 0.0)

# -- three imaginary time gf

imtime = MeshImTime(beta=beta, S='Fermion', n_max=ntau)
prodmesh = MeshProduct(imtime, imtime, imtime)
g4_tau = Gf(name='g4_tau', mesh=prodmesh, indices=[1])

for (i1, t1), (i2, t2), (i3, t3) in itertools.product(*[
        enumerate(mesh) for mesh in g4_tau.mesh.components]):

    t1, t2, t3 = t1.real, t2.real, t3.real # Can we get rid of this?    
    g4_tau[[i1, i2, i3]][:] = g_tau(t1-t2)*g_tau(t3) - g_tau(t1)*g_tau(t3-t2)

    
for (i1, t1), (i2, t2), (i3, t3) in itertools.product(*[
        enumerate(mesh) for mesh in g4_tau.mesh.components]):

    t1, t2, t3 = t1.real, t2.real, t3.real # Can we get rid of this?    

    #val = g4_tau(t1, t2, t3) # Not implemented FIXME
    val = g4_tau[[i1, i2, i3]]
    val_ref = g_tau(t1-t2)*g_tau(t3) - g_tau(t1)*g_tau(t3-t2)

    np.testing.assert_array_almost_equal(val, val_ref)
    
