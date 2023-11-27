# Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2020 Simons Foundation
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
# Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

from triqs.gf import *
from triqs.gf.tools import fit_legendre
from numpy import *

def are_close(a,b):
  return abs(a-b)<1e-10

beta = 10.0
n = 2

g_iw = GfImFreq(beta=beta, target_shape=[])
g_iw << iOmega_n

assert are_close(g_iw(n), 1j*(2*n+1)*pi/beta)

g_iw.invert()
g_tau = GfImTime(beta=beta, target_shape=[])
g_tau << Fourier(g_iw)
g_tau_real = g_tau.real

assert are_close(g_tau(1.), -0.5)
assert are_close(g_tau_real(1.), -0.5)

g_l = fit_legendre(g_tau)
assert (are_close(g_l(0.5), -0.5))

g_w = GfReFreq(indices=[0], window=[0.,1.], n_points = 1000)
g_w << Omega+0.1

assert are_close(g_w(0.5), 0.6)
