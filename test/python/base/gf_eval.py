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
from numpy import *

def are_close(a,b):
  return abs(a-b)<1e-10

beta = 10.0
n = 2

g_iw = GfImFreq(beta=beta, indices=[0])
g_iw << iOmega_n


assert are_close(1j*(2*n+1)*pi/beta, g_iw(n)[0,0])

g_iw.invert()
g_tau = GfImTime(beta=beta, indices=[0])
g_tau << Fourier(g_iw)

assert are_close(g_tau(1.)[0,0], -0.5)

 #FIXME : PUT BACK
# g_w = GfReFreq(indices=[0], window=[0.,1.], n_points = 1000)
# g_w << Omega+0.1

# assert are_close(g_w(0.5)[0,0], 0.6)

# g_w.invert()
# g_t = GfReTime(indices=[0], window=[0.,2*pi*999/(1+1/999.)], n_points = 1000)
# g_t << Fourier(g_w)

# assert are_close(g_t(1.)[0,0], -0.926625205354-0.0852181350513j)

