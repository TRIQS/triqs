from numpy import *
from numpy.linalg import *
#from matplotlib.pyplot import *
from pytriqs.gf.local import *
from pytriqs.lattice import *
units =[
        matrix([[1.,0.,0.],[0.,1.,0.],[0.,0.,1.]], float), #square lattice
        matrix([[1.,0.,0.],[0.5,sqrt(3)/2.,0.],[0.,0.,1.]], float)  #triangular lattice
       ]
per_mats = [
     matrix([[16,0,0],[0,16,0],[0,0,1]], int32),
     matrix([[2,0,0],[0,4,0],[0,0,1]], int32),
     matrix([[1,2,0],[-2,1,0],[0,0,1]], int32),
     matrix([[6,6,0],[-6,6,0],[0,0,1]], int32)
]
        
#gs=GridSpec(2,4)
#isub=0
for unit in units:
 bz=BrillouinZone(BravaisLattice(unit))
 for per_mat in per_mats:
#  subplot(gs[isub], aspect="equal");isub+=1
#  title(r"$N_c=%s$"%(det(per_mat)))
  k_mesh = MeshBrillouinZone(bz, per_mat)
#  for k in k_mesh:
#    plot(k[0],k[1],'or')
#  xticks([0,pi,2*pi])  
#  yticks([0,pi,2*pi])  
#  #xticklabels(["0",r"$\pi$",r"$2\pi$"])  
#  grid()
#tight_layout()
assert k_mesh.locate_neighbours([0.,5.,0.]) == [4,4,0]
assert k_mesh.locate_neighbours([2.,1.,0.]) == [4, 0, 0]
assert k_mesh.locate_neighbours([2.,0.,0.]) == [3, -1, 0]

assert k_mesh.index_to_linear(k_mesh.locate_neighbours([2.,1.,0.])) == 24
assert k_mesh.index_to_linear(k_mesh.locate_neighbours([2.,0.,0.])) == 23
