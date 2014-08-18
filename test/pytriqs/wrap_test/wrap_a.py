from my_module import *

a = Ac()
print a.m1(1,2.0)
print a.m1p(1,2.0)

print_a (a)

v1 =  make_vector(3)
v2 = vector_x2(v1)

print v1, v2

print vector_x2([1,2,3,4])
print vector_x2((1,2,3))

print a+ a
#print_a (8)

f,s = a.__reduce__()
b = f(*s)

# lambda <-> std::function
fc = make_fnt_ii()
#print fc
use_fnt_ii(fc)
fp = lambda i,j : i + 20*j
use_fnt_ii(fp)

fc2 = make_fnt_iid()
#print fc2
use_fnt_iid(fc2)
fp2 = lambda i,j,a : a+ i + 20*j
use_fnt_iid(fp2)

print map_to_mapvec({'a':1, "b":2, "sjkdf":5})

from pytriqs.archive import *
import pytriqs.archive.hdf_archive_schemes

import numpy
R = HDFArchive('ess.h5', 'w')    # Opens the file myfile.h5, in read/write mode
R['a'] = 2.3*a
del R

R2 = HDFArchive('ess.h5', 'r') 
aa = R2['a']
print " rereading from hdf5 ", aa

import pickle
s = pickle.dumps(a)
print repr(s)
bb = pickle.loads(s)
print bb


