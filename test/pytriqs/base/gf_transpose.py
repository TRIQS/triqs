from pytriqs.gf.local import *

g = GfImFreq(indicesR = range(5), indicesL = range(3), beta = 40, n_points = 1000)
gt = g.transpose()

print "Old data shape = ", g.data.shape
print "Old tail shape = ", g.tail.data.shape
print "New data shape = ", gt.data.shape
print "New tail shape = ", gt.tail.data.shape
