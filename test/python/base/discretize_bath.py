# Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
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
# Authors: Alexander Hampel, Nils Wentzell

import numpy as np
from triqs.gf import *
from triqs.gf.tools import discretize_bath
from triqs.utility.comparison_tests import *

# build delta from discretized values
delta_iw = GfImFreq(target_shape=[], mesh=MeshImFreq(beta=40, S='Fermion', n_max=200))

hoppings = np.array([0.2, 0.6])
energies = np.array([0.0, 0.5])

for w in delta_iw.mesh:
    delta_iw[w] = np.sum(hoppings**2/( w.value - energies ) )

# run bath fitter on this input
t_opt, e_opt, delta_disc_iw = discretize_bath(delta_iw = delta_iw, Nb = 2, bandwidth=2.5, t_init=0.1, tol=1e-8)

# compare to given values
assert max(abs(hoppings - t_opt)) < 1e-8, 'did not achieved requiered accuracy for bath fit '+str(t_opt)+' vs '+str(hoppings)
assert max(abs(energies - e_opt)) < 1e-8, 'did not achieved requiered accuracy for bath fit '+str(e_opt)+' vs '+str(energies)
assert_gfs_are_close(delta_disc_iw, delta_iw)
