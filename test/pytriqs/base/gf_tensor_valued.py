from pytriqs.gf.local import *
from pytriqs.archive import *
from pytriqs.plot.mpl_interface import *

#one constructor
m=MeshImFreq(beta=1, S='Fermion', n_max=10)
G=GfImFreqTv3(mesh=m, indices=[['a'],['b1','b2'],['c1', 'c2']])

#another constructor
G2=GfImFreqTv3(beta=1.,statistic="Fermion",n_points=100, indices=[['a'],['b1','b2'],['c1', 'c2']])

assert G.data.shape==(20,1,2,2),"not ok"
assert G['a','b1','c2'].data.shape==(20,1,1,1), "not ok"

#H5 r/w
A=HDFArchive("Tv3.h5",'w')
A["G"]=G
del A

A2=HDFArchive("Tv3.h5",'r')
G3=A2["G"]
del A2
assert G3.data.shape==G.data.shape,"not ok:%s vs %s"%(G3.data.shape, G.data.shape)
