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
from triqs.gf.tools import discretize_bath, make_delta
from triqs.utility.comparison_tests import *

# build delta from discretized values
mesh=MeshImFreq(beta=40, S='Fermion', n_max=200)

hoppings = np.array([0.2, 0.6])
energies = np.array([0.0, 0.5])

delta_iw = make_delta(V= hoppings, eps=energies, mesh=mesh)

# run bath fitter on this input
V_opt, e_opt, delta_disc_iw = discretize_bath(delta_in = delta_iw, Nb = 2, eps0=2.5, V0=0.1, tol=1e-8)

# compare to given values
assert max(abs(hoppings - V_opt)) < 1e-8, 'did not achieved requiered accuracy for bath fit '+str(V_opt)+' vs '+str(hoppings)
assert max(abs(energies - e_opt)) < 1e-8, 'did not achieved requiered accuracy for bath fit '+str(e_opt)+' vs '+str(energies)
assert_gfs_are_close(delta_disc_iw, delta_iw)


# second test with 2x2 delta input and init guess
mesh=MeshImTime(beta=40, S='Fermion', n_max=1001)

hoppings = np.array([[ 6.1916012067e-01,  1.4280122215e-08],
                     [-1.9899876996e-07,  3.3503877422e-01],
                     [ 3.4998359745e-01,  1.8024665743e-07],
                     [-4.0523734143e-08,  1.8849071562e-01],
                     [ 1.3545326772e-01,  4.9300282564e-08],
                     [ 3.8188035741e-08, -2.2889123931e-01],
                     [ 2.4378742241e-01,  3.3385121561e-08],
                     [ 6.9810984451e-06, -4.7688592983e-02]])
energies = np.array([-1.5437759000e+00, -4.0244628425e-01, -3.3678965769e-01, -3.8795127563e-02, 
                     9.9602047476e-03,  1.6012620754e-01,  3.4692574848e-01,  2.0529795026e+03])

delta_tau = make_delta(V= hoppings, eps=energies, mesh=mesh)

# run bath fitter on this input
V_opt, e_opt, delta_disc_tau = discretize_bath(delta_in = delta_tau, Nb = 8, eps0= energies, V0=hoppings, tol=1e-6, maxiter=100000)

# # compare to given values
assert np.max(np.abs(hoppings - V_opt)) < 1e-6, 'did not achieved requiered accuracy for bath fit '+str(V_opt)+' vs '+str(hoppings)
assert np.max(np.abs(energies - e_opt)) < 1e-6, 'did not achieved requiered accuracy for bath fit '+str(e_opt)+' vs '+str(energies)
assert_gfs_are_close(delta_disc_tau, delta_tau)


