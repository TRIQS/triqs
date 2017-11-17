from pytriqs.gf import *

g = GfImFreq(beta=10, indices=[0,1,2])
print g.indices[0]
print g[0:2,0:1].indices[0], g[0:2,0:1].indices[1]

g = GfImFreq(beta=10, indices=['a','b','c'])
print g.indices[0]
print g[0:2,0:1].indices[0], g[0:2,0:1].indices[1]
