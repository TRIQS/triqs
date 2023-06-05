# Copyright (c) 2019-2022 Simons Foundation
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
# Authors: Nils Wentzell

from triqs.gf import *

# ==== Matrix-valued Green functions

n_w = 1001
w_mesh = MeshReFreq(w_min=-10, w_max=10, n_w = n_w)
gw = Gf(mesh=w_mesh, target_shape=(1,1))
gw2 = gw.copy()
#gw << SemiCircular(2.0)
gw << inverse(Omega - 1 + 1j * 0.001)

t_mesh = make_adjoint_mesh(w_mesh)
gt2 = Gf(mesh=t_mesh, target_shape=(1,1))

# ======== Without tail-fitting ========

gt2 << Fourier(gw)
gt3 = make_gf_from_fourier(gw)

# Check that forward and backward transform gives Identity
gw2 << Fourier(gt2)
gw3 = make_gf_from_fourier(gt3)
print(max(abs(gw.data[:] - gw2.data[:])))
print(max(abs(gw.data[:] - gw3.data[:])))
assert max(abs(gw.data[:] - gw2.data[:])) < 1e-7
assert max(abs(gw.data[:] - gw3.data[:])) < 1e-7

# Check Fourier transform of real frequency Green function
gw3 = make_gf_from_fourier(gt3.real) + 1j * make_gf_from_fourier(gt3.imag)
print(max(abs(gw.data[:] - gw3.data[:])))
assert max(abs(gw.data[:] - gw3.data[:])) < 1e-7

# ======== With tail-fitting ========

tail, err = fit_tail(gw, make_zero_tail(gw, 1))
print(tail, err)
gt2 << Fourier(gw, tail)
gt3 = make_gf_from_fourier(gw, t_mesh, tail)

# Check that forward and backward transform gives Identity
gw2 << Fourier(gt2, tail)
gw3 = make_gf_from_fourier(gt3, w_mesh, tail)
print(max(abs(gw.data[:] - gw2.data[:])))
print(max(abs(gw.data[:] - gw3.data[:])))
assert max(abs(gw.data[:] - gw2.data[:])) < 1e-7
assert max(abs(gw.data[:] - gw3.data[:])) < 1e-7
