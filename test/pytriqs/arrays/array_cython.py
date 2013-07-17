#import pytriqs.tools.test.array_cython as M
import array_cython as M
import numpy as np,sys

a = np.array([[1,2],[3,4]])
b = np.array( np.transpose( np.array([a, 10*a,100*a]), (1,2,0)), order = 'C')
c =  b.transpose(1,0,2)
M.f(b)
M.f(c)

print " ------------ F --------------"

b = np.array( np.transpose( np.array([a, 10*a,100*a]), (1,2,0)), order = 'F')
c =  b.transpose(1,0,2)
M.f(b)
M.f(c)

d = b.transpose(1,2,0)
M.f(d)

dd = b.transpose(2,0,1)
M.f(dd)

#print "ref b ", sys.getrefcount(b)
#M.f(b)
#print "ref b ", sys.getrefcount(b)

