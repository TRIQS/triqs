################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011-2014 by M. Ferrero, O. Parcollet
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
from pytriqs.utility.comparison_tests import *
import numpy as np, copy
from math import pi

beta = 50
precision = 1.e-6

def matsu(n) : 
   return (2*n+1)*pi/beta * 1j

def max_abs(a) : 
    return np.amax(np.abs(a)) 

# --- start test

ga = GfImFreq(indices = [1,2], beta = beta, n_points = 100, name = "a1Block")
gb = GfImFreq(indices = [1,2], beta = beta, n_points = 100, name = "b1Block")

G = BlockGf(name_list = ('a','b'), block_list = (ga,gb), make_copies = False)
G << iOmega_n + 2.0

#print G['a'].tail.data 

for ii, g in G : 
    N = g.data.shape[0]
    for n in range(N/2) : 
        assert_array_close_to_scalar( g.data[n+N/2],  matsu(n) + 2.0)

    assert_array_close_to_scalar(g.tail[-1],1)
    assert_array_close_to_scalar(g.tail[0],2)
    assert max_abs(g.tail.data[-g.tail.order_min + 1:]) < precision, "oops"

# inverse:
G << inverse(G)

#  Density:
dens = G.total_density()
assert abs(dens - 4.00125926188) < precision, "oops"

# FT:
f = lambda g,L : GfImTime(indices = g.indices, beta = g.mesh.beta, n_points =L )
gt = BlockGf(name_block_generator = [ (n,f(g,201) ) for n,g in G], make_copies=False, name='gt')
for (i,gtt) in gt : gtt.set_from_inverse_fourier(G[i])

res = np.array([[[  3.14815470e-04,   0.00000000e+00],
        [  0.00000000e+00,   3.14815470e-04]],

       [[ -1.48721028e-04,   0.00000000e+00],
        [  0.00000000e+00,  -1.48721028e-04]],

       [[  7.46732524e-05,   0.00000000e+00],
        [  0.00000000e+00,   7.46732524e-05]]])

assert_arrays_are_close(gt['a'].data[:3], res) 

# Matrix operations:
ga2 = GfImFreq(indices = [1,2,3], beta = beta, n_points = 100, name = "a1Block")
mat = np.array([[1.0,0.0,1.0],[-1.0,1.0,0.0]], np.complex)

ga2.from_L_G_R(mat.transpose(),ga,mat)

res = np.array([[[ 0.99901401-0.03138495j, -0.49950701+0.01569248j,
          0.49950701-0.01569248j],
        [-0.49950701+0.01569248j,  0.49950701-0.01569248j,  0.00000000+0.j        ],
        [ 0.49950701-0.01569248j,  0.00000000+0.j        ,
          0.49950701-0.01569248j]],

       [[ 0.99119556-0.09341798j, -0.49559778+0.04670899j,
          0.49559778-0.04670899j],
        [-0.49559778+0.04670899j,  0.49559778-0.04670899j,  0.00000000+0.j        ],
        [ 0.49559778-0.04670899j,  0.00000000+0.j        ,
          0.49559778-0.04670899j]],

       [[ 0.97592014-0.15329718j, -0.48796007+0.07664859j,
          0.48796007-0.07664859j],
        [-0.48796007+0.07664859j,  0.48796007-0.07664859j,  0.00000000+0.j        ],
        [ 0.48796007-0.07664859j,  0.00000000+0.j        ,
          0.48796007-0.07664859j]]])
shift = ga2.data.shape[0]/2
assert_arrays_are_close(ga2.data[shift:shift+3], res) 

# conjugate:
Gc = G.conjugate()

res = np.array([[[ 0.49950701+0.01569248j,  0.00000000-0.j        ],
        [-0.00000000-0.j        ,  0.49950701+0.01569248j]],

       [[ 0.49559778+0.04670899j,  0.00000000-0.j        ],
        [-0.00000000-0.j        ,  0.49559778+0.04670899j]],

       [[ 0.48796007+0.07664859j,  0.00000000-0.j        ],
        [-0.00000000-0.j        ,  0.48796007+0.07664859j]]])
shift = Gc['a'].data.shape[0]/2
assert_arrays_are_close(Gc['a'].data[shift:shift+3], res) 

# to be continued
# tranpose

with HDFArchive('gf_base_op_test.h5','w') as h : 
   h['g'] = G
   h['gt'] = gt

# Check reading out of archive
with HDFArchive('gf_base_op_test.h5','r') as h : 
   assert_block_gfs_are_close(h['g'], G)
   assert_block_gfs_are_close(h['gt'], gt)
 
# Pickle (also use in mpi, etc...)
import pickle

def check_pickle(g): 
  s = pickle.dumps(g)
  g2 = pickle.loads(s)
  assert_block_gfs_are_close(g,g2)

check_pickle(G)
check_pickle(gt)

