from numpy import *
from numpy.linalg import *
from triqs.gf import *
from triqs.lattice import *
from math import pi

# ---- square lattice : 16 x 16 x 1
units = matrix([[1.,0.,0.],[0.,1.,0.],[0.,0.,1.]], float)
per_mat = matrix([[16, 0, 0],[0, 16, 0],[0, 0, 1]], int32)
bz=BrillouinZone(BravaisLattice(units))
k_mesh = MeshBrillouinZone(bz, per_mat)

assert all(k_mesh.locate_neighbours([pi/2,  pi, 0.0]) == [4, 8, 0])
assert all(k_mesh.locate_neighbours([pi/8,2*pi, 0.0]) == [1, 16, 0])

# ---- triangular lattice : 6 x 6 x 1
units = matrix([[1.,0.,0.],[0.5,sqrt(3)/2.,0.],[0.,0.,1.]], float)
per_mat=  matrix([[6, 6, 0],[-6, 6, 0],[0, 0, 1]], int32)
bz=BrillouinZone(BravaisLattice(units))
k_mesh = MeshBrillouinZone(bz, per_mat)

assert all(k_mesh.locate_neighbours([0.,5.,0.]) == [4, 4, 0])
assert all(k_mesh.locate_neighbours([2.,1.,0.]) == [4, 0, 0])
assert all(k_mesh.locate_neighbours([2.,0.,0.]) == [3, -1, 0])
