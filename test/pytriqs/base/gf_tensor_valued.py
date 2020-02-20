from __future__ import print_function
from pytriqs.gf import *
from pytriqs.archive import *

#one constructor
m=MeshImFreq(beta=1, S='Fermion', n_max=10)
G=GfImFreq(mesh=m, indices=[['a'],['b1','b2'],['c1', 'c2']])

#another constructor
G2=GfImFreq(beta=1.,statistic="Fermion",n_points=100, indices=[['a'],['b1','b2'],['c1', 'c2']])

assert G.data.shape==(20,1,2,2),"not ok"
assert G['a','b1','c2'].data.shape==(20,), "not ok"

#H5 r/w
A=HDFArchive("Tv3.h5",'w')
A["G"]=G
del A

A2=HDFArchive("Tv3.h5",'r')
G3=A2["G"]
del A2
assert G3.data.shape==G.data.shape,"not ok:%s vs %s"%(G3.data.shape, G.data.shape)

#mpi bcast
import pytriqs.utility.mpi as mpi
G4=GfImFreq(beta=1.,statistic="Fermion",n_points=100, indices=[['a'],['b1','b2'],['c1', 'c2']])
if mpi.is_master_node():
   G4.data[:,:,:,:] = 5
   assert G4.data[0,0,0,0]==5, "not ok :%s"%(G4.data[0,0,0,0])
if not mpi.is_master_node():
   assert G4.data[0,0,0,0]==0, "not ok"
G4 = mpi.bcast(G4)
if not mpi.is_master_node():
   assert G4.data[0,0,0,0]==5, "not ok :%s"%(G4.data[0,0,0,0])


##Tv4
print("#############################")
G5=GfImFreq(mesh=m, indices=[['a'],['b1','b2'],['c1', 'c2'], ['c']])
print(G5.data.shape)

assert G5.data.shape==(20,1,2,2,1),"not ok"
assert G5['a','b1','c2', 'c'].data.shape==(20,), "not ok"

#ImTime, 
print("#############################")
G6=GfImTime(beta=1.,statistic="Fermion",n_points=100, indices=[['a'],['b1','b2'],['c1', 'c2']])
