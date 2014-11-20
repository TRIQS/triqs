
################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011 by M. Ferrero, O. Parcollet
#
# TRIQS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# TRIQS. If not, see <http://www.gnu.org/licenses/>.
#
################################################################################

from pytriqs.archive import *
from pytriqs.gf.local import *
import numpy, copy

h=HDFArchive('gf_base_op.output.h5','w')

ga = GfImFreq(indices = [1,2], beta = 50, n_points = 100, name = "a1Block")
gb = GfImFreq(indices = [1,2], beta = 50, n_points = 100, name = "b1Block")

G = BlockGf(name_list = ('a','b'), block_list = (ga,gb), make_copies = False)

G << iOmega_n + 2.0
h['G1'] = G

# inverse:
G << inverse(G)

#  Density:
dens = G.total_density()
h['dens'] = dens

# FT:
f = lambda g,L : GfImTime(indices = g.indices, beta = g.mesh.beta, n_points =L )
gt = BlockGf(name_block_generator = [ (n,f(g,200) ) for n,g in G], make_copies=False, name='gt')
for (i,gtt) in gt : gtt.set_from_inverse_fourier(G[i])

h['gt'] = gt

# Matrix operations:
ga2 = GfImFreq(indices = [1,2,3], beta = 50, n_points = 100, name = "a1Block")
mat = numpy.array([[1.0,0.0,1.0],[-1.0,1.0,0.0]], numpy.complex)

ga2.from_L_G_R(mat.transpose(),ga,mat)

h['ga2'] = ga2


# conjugate:
Gc = G.conjugate()
h['Gc'] = Gc

# tranpose
g = G['a']
if 0 : 
    gt = G['a'].transpose()
    gt.data[3,0,1] = 100
    assert g.data[3,1,0] == 100

del h
