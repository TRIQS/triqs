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
from triqs.gf import MeshImFreq, MeshImTime
from triqs.gf.tools import discretize_bath, make_delta
from triqs.utility.comparison_tests import assert_gfs_are_close

# build delta from discretized values
mesh = MeshImFreq(beta=40, S='Fermion', n_max=200)

hoppings = np.array([[0.2, 0.6]])
energies = np.array([0.0, 0.5])

delta_iw = make_delta(V=hoppings, eps=energies, mesh=mesh)

# run bath fitter on this input
V_opt, e_opt, delta_disc_iw = discretize_bath(delta_in=delta_iw, Nb=2, eps0=2.5, V0=None, tol=1e-10)
# compare to given values , resulting V can be correct up to a global sign
assert np.max(np.abs(hoppings) - np.abs(V_opt)) < 1e-6, 'did not achieved requiered accuracy for bath fit \n'+str(V_opt)+' vs \n'+str(hoppings)
assert np.max(np.abs(energies - e_opt)) < 1e-6, 'did not achieved requiered accuracy for bath fit \n'+str(e_opt)+' vs \n'+str(energies)
assert_gfs_are_close(delta_disc_iw, delta_iw)

# try with Nb % n_orb != 0
V_opt, e_opt, delta_disc_iw = discretize_bath(delta_in=delta_iw, Nb=3, eps0=2.5, V0=None, tol=1e-10)
V_opt, e_opt, delta_disc_iw = discretize_bath(delta_in=delta_iw, Nb=1, eps0=2.5, V0=None, tol=1e-10)

#################################################
# second test with 2x2 delta input and init guess
mesh = MeshImTime(beta=40, S='Fermion', n_max=1001)

hoppings = np.array([[6.1916012067e-01, 0.0, 3.4998359745e-01, 0.0, 1.3545326772e-01, 0.0, 2.4378742241e-01, 0.0],
                     [0.0, 3.3503877422e-01, 0.0, 1.8849071562e-01, 0.0, -2.2889123931e-01, 0.0, -4.7688592983e-02]])

energies = np.array([-1.5437759000e+00, -4.0244628425e-01, -3.3678965769e-01, -3.8795127563e-02,
                     9.9602047476e-03,  1.6012620754e-01,  3.4692574848e-01,  2.0529795026e+03])

delta_tau = make_delta(V=hoppings, eps=energies, mesh=mesh)

V_opt, e_opt, delta_disc_tau = discretize_bath(delta_in=delta_tau, Nb=8, eps0=energies, V0=None, tol=1e-12, maxiter=1000000)
print('max hopping deviation', np.max(np.abs(hoppings) - np.abs(V_opt)))

# compare to given values
assert np.max(np.abs(hoppings) - np.abs(V_opt)) < 1e-6, 'did not achieved requiered accuracy for bath fit \n'+str(V_opt)+' vs \n'+str(hoppings)
assert np.max(np.abs(energies - e_opt)) < 1e-6, 'did not achieved requiered accuracy for bath fit \n'+str(e_opt)+' vs \n'+str(energies)
assert_gfs_are_close(delta_disc_tau, delta_tau)


#################################################
# test blockGf
mesh = MeshImTime(beta=40, S='Fermion', n_max=501)

hoppings = [np.array([[0.2, 0.1, -0.5, 0.15]]),
            np.array([[0.2, -0.35, 0.7, 0.1]])]

energies = [np.array([-2.2, -1.1, 0.0, 0.7]),
            np.array([-1.7, -0.3, -0.1, 0.2])]

delta_tau = make_delta(V=hoppings, eps=energies, mesh=mesh)

# run bath fitter on hopping as input
V_opt, e_opt, delta_disc_tau = discretize_bath(delta_in=delta_tau, Nb=4, eps0=energies, V0=hoppings, tol=1e-12, maxiter=100000)
print('max hopping deviation', np.max(np.abs(hoppings) - np.abs(V_opt)))

# compare to given values
for i, (block, delta_disc) in zip(range(len(list(delta_disc_tau.indices))), delta_disc_tau):
    assert np.max(np.abs(hoppings[i]) - np.abs(V_opt[i])) < 1e-8, 'did not achieved requiered accuracy for bath fit \n'+str(V_opt)+' vs \n'+str(hoppings)
    assert np.max(np.abs(energies[i] - e_opt[i])) < 1e-8, 'did not achieved requiered accuracy for bath fit \n'+str(e_opt)+' vs \n'+str(energies)
    assert_gfs_are_close(delta_disc, delta_tau[block])


#################################################
# and test without providing input parameters
V_opt, e_opt, delta_disc_tau = discretize_bath(delta_in=delta_tau, Nb=4, eps0=2, V0=None, tol=1e-15, maxiter=200, method='basinhopping')
print('max hopping deviation', np.max(np.abs(hoppings) - np.abs(V_opt)))

# compare to given values
for i, (block, delta_disc) in zip(range(len(list(delta_disc_tau.indices))), delta_disc_tau):
    assert np.max(np.abs(hoppings[i]) - np.abs(V_opt[i])) < 1e-3, 'did not achieved requiered accuracy for bath fit \n'+str(V_opt)+' vs \n'+str(hoppings)
    assert np.max(np.abs(energies[i] - e_opt[i])) < 1e-3, 'did not achieved requiered accuracy for bath fit \n'+str(e_opt)+' vs \n'+str(energies)
    assert_gfs_are_close(delta_disc, delta_tau[block], precision=1e-3)
