# Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2022 Simons Foundation
# Copyright (c) 2017 Hugo U.R. Strand
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You may obtain a copy of the License at
#     https:#www.gnu.org/licenses/gpl-3.0.txt
#
# Authors: Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell

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
from triqs.gf import Gf, GfImTime, MeshImTime, MeshProduct

ntau = 10
beta = 1.2345

g_tau = GfImTime(name='g_tau', beta=beta, statistic='Fermion', n_points=ntau, indices=[1])

tau = np.array([tau.value for tau in g_tau.mesh])
g_ref = np.exp(-beta * tau)
g_tau.data[:, 0, 0] = np.exp(-beta * tau)

for tau in g_tau.mesh:
    g_tau[tau] = np.exp(-beta * tau)

for idx, tau in enumerate(g_tau.mesh):

    # comparison does not work at beta since the evaluation g_tau() wraps..
    if idx == len(g_tau.mesh)-1: break
    
    #diff_interp = g_tau(tau)[0,0] - g_ref[idx] # FIXME: tau is complex
    
    diff_interp = g_tau(tau.real)[0,0] - g_ref[idx]
    diff_dbrack = g_tau[tau][0,0] - g_ref[idx]

    np.testing.assert_almost_equal(diff_interp, 0.0)
    np.testing.assert_almost_equal(diff_dbrack, 0.0)

# -- three imaginary time gf

imtime = MeshImTime(beta=beta, S='Fermion', n_tau=ntau)
g4_tau = Gf(name='g4_tau', mesh= MeshProduct(imtime, imtime, imtime), indices=[1])

for t1, t2, t3 in g4_tau.mesh:
    g4_tau[t1, t2, t3] = g_tau(t1)*g_tau(t3) - g_tau(t1)*g_tau(t3)

for t1, t2, t3 in g4_tau.mesh:
    val = g4_tau[t1, t2, t3] 
    val_ref = g_tau(t1)*g_tau(t3) - g_tau(t1)*g_tau(t3)
    np.testing.assert_array_almost_equal(val, val_ref)

