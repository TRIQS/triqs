# Copyright (c) 2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2017 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020 Simons Foundation
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

from triqs.gf import *

# A Green's function on the Matsubara axis set to a semicircular
gw = GfImFreq(indices = [1], beta = 50)
gw << SemiCircular(half_bandwidth = 1)

# Create a Legendre Green's function with 40 coefficients
# initialize it from gw and plot it
gl = GfLegendre(indices = [1], beta = 50, n_points = 40)
gl << MatsubaraToLegendre(gw)

