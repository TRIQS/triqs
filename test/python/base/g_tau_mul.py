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

from h5 import *
from triqs.gf import *
from triqs.utility.comparison_tests import *
from triqs.lattice import *
from triqs.lattice.tight_binding import *
from numpy import cos
import numpy as np, copy
from math import pi

BL = BravaisLattice(units = [(1,0,0) , (0,1,0) ]) #square lattice
nk=20
mk = MeshBrZone(BrillouinZone(BL), nk)

mtf=MeshImTime (beta=1., S="Fermion", n_tau=100)
mtb=MeshImTime (beta=1., S="Boson", n_tau=100)

f= Gf(mesh = mtf, target_shape = [1,1])
b= Gf(mesh = mtb, target_shape = [1,1])

class TestError(Exception):
  pass

# === Assert failure of p = f + b and f *= b

try:
  p = f + b
  raise TestError
except (RuntimeError, AssertionError) as e:
  pass

try:
  f *= b
  raise TestError
except (RuntimeError, AssertionError) as e:
  pass


# === Test G(tau) Products

p = f* f
assert p.mesh.statistic == 'Boson'
p = f* b
assert p.mesh.statistic == 'Fermion'
p = b* f
assert p.mesh.statistic == 'Fermion'
p = b* b
assert p.mesh.statistic == 'Boson'


# === Test G(tau1, tau2) Products

ff= Gf(mesh =  MeshProduct(mtf, mtf), target_shape = [1,1])
fb= Gf(mesh =  MeshProduct(mtf, mtb), target_shape = [1,1])

p = ff* ff
assert p.mesh[0].statistic == 'Boson'
assert p.mesh[1].statistic == 'Boson'

p = ff* fb
assert p.mesh[0].statistic == 'Boson'
assert p.mesh[1].statistic == 'Fermion'

p = fb* ff
assert p.mesh[0].statistic == 'Boson'
assert p.mesh[1].statistic == 'Fermion'

p = fb* fb
assert p.mesh[0].statistic == 'Boson'
assert p.mesh[1].statistic == 'Boson'


# === Test G(k, tau) Products

f = Gf(mesh =  MeshProduct(mk, mtf) , target_shape = [1,1])
b = Gf(mesh =  MeshProduct(mk, mtb) , target_shape = [1,1])

p = f + f
assert p.mesh[1].statistic == 'Fermion'

p = b + b
assert p.mesh[1].statistic == 'Boson'

p = f* f
assert p.mesh[1].statistic == 'Boson'
p = f* b
assert p.mesh[1].statistic == 'Fermion'
p = b* f
assert p.mesh[1].statistic == 'Fermion'
p = b* b
assert p.mesh[1].statistic == 'Boson'

