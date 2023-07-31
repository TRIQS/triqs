# Copyright (c) 2013-2015 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2015 Centre national de la recherche scientifique (CNRS)
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
# Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell


from numpy import array, arange
from math import pi
from cmath import sqrt, log
from triqs.gf import *
from triqs.gf.descriptors import Function
from triqs.utility.comparison_tests import *

beta = 100  # Inverse temperature
L = 10      # Number of Matsubara frequencies used in the Pade approximation
eta = 0.01  # Imaginary frequency shift

## Test Green's functions ##

# Two Lorentzians
def GLorentz(z):
    return 0.7/(z-2.6+0.3*1j) + 0.3/(z+3.4+0.1*1j)

# Semicircle
def GSC(z):
    return 2.0*(z + sqrt(1-z**2)*(log(1-z) - log(-1+z))/pi)

# A superposition of GLorentz(z) and GSC(z) with equal weights
def G(z):
    return 0.5*GLorentz(z) + 0.5*GSC(z)

for sh in [(), (2,), (1,1), (2,2), (2,2,2), (2,2,2,2)]:
    # Matsubara GF
    gm = GfImFreq(target_shape=sh, beta = beta, name = "gm")

    scalar_valued = (len(sh) == 0)

    idx0 = next(gm.target_indices)
    for idx in gm.target_indices:
        if scalar_valued: gm << Function(G)
        else:
            gm[idx] << (1 + sum(idx)) * Function(G)
            # Compare against first element
            assert_gfs_are_close((1 + sum(idx)) * gm[idx0], gm[idx])

    # Analytic continuation of gm
    g_pade = GfReFreq(target_shape=sh, window = (-5.995, 5.995), n_points = 1200, name = "g_pade")
    g_pade.set_from_pade(gm, n_points = L, freq_offset = eta)
    if not scalar_valued:
        for idx in gm.target_indices:
             assert_gfs_are_close((1 + sum(idx)) * g_pade[idx0], g_pade[idx])

    if sh == (1,1):
        from h5 import HDFArchive
        R = HDFArchive('pade.ref.h5','r')
        assert_gfs_are_close(g_pade, R['g_pade'])
