# Copyright (c) 2013-2014 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2014 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2019-2020 Simons Foundation
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
# Authors: Olivier Parcollet, Nils Wentzell

# Import the Green's functions
from triqs.gf import GfImFreq, iOmega_n, inverse

# Create the Matsubara-frequency Green's function and initialize it
g = GfImFreq(indices = [1], beta = 50, n_points = 1000, name = "imp")
g << inverse( iOmega_n + 0.5 )

import triqs.utility.mpi as mpi

mpi.bcast(g)



#Block

from triqs.gf import *
g1 = GfImFreq(indices = ['eg1','eg2'], beta = 50, n_points = 1000, name = "egBlock")
g2 = GfImFreq(indices = ['t2g1','t2g2','t2g3'], beta = 50, n_points = 1000, name = "t2gBlock")
G = BlockGf(name_list = ('eg','t2g'), block_list = (g1,g2), make_copies = False)


mpi.bcast(G)


#imtime 
from triqs.gf import *

# A Green's function on the Matsubara axis set to a semicircular
gw = GfImFreq(indices = [1], beta = 50)
gw << SemiCircular(half_bandwidth = 1)

# Create an imaginary-time Green's function and plot it
gt = GfImTime(indices = [1], beta = 50)
gt << Fourier(gw)

mpi.bcast(gt)


##

gt2 = GfImTime(indices = [1], beta = 50)

gt2 = mpi.bcast(gt2)

gw2 = GfImFreq(indices = [1], beta = 50)
gw2 << Fourier(gt2)


