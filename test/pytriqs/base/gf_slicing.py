from pytriqs.gf import *

g = GfImFreq(beta=10, indices=[0,1,2])
print g.indices
print g[0:2,0:1].indicesL, g[0:2,0:1].indicesR
print g['0','2'].indicesL, g['0','2'].indicesR

g = GfImFreq(beta=10, indices=['a','b','c'])
print g.indices
print g[0:2,0:1].indicesL, g[0:2,0:1].indicesR
print g['a','c'].indicesL, g['a','c'].indicesR
