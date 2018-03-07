
""" Constuct a random histogram and broad cast it 
and compare the broadcasted result with a reference """

import numpy as np
import pytriqs.utility.mpi as mpi
from pytriqs.statistics.histograms import Histogram

def py_histogram(int_vec):
    
    hist = {}
    for val in int_vec:
        hist[val] = 1 + hist.get(val, 0)

    return hist

if mpi.is_master_node():

    x = np.random.randint(0, high=10, size=100)
    
    h_ref = py_histogram(x)
    h = Histogram(0, 10)
    h << x

else:
    h, h_ref = None, None
    
h = mpi.bcast(h)
h_ref = mpi.bcast(h_ref)

for rank in xrange(mpi.size):
    if rank == mpi.rank:

        print '-'*72
        print 'rank =', mpi.rank
        print 'h =\n', h
        print 'h_ref =\n', h_ref

        # -- Compare h and h_ref
        pts = np.array([ int(h.mesh_point(idx)) for idx in xrange(len(h.data))])

        for pt, val in zip(pts, h.data):
            val = int(val)
            #print pt, val
            if val > 0:
                assert( val == h_ref[pt] )
            else:
                assert( not h_ref.has_key(pt) )
        
        for key, value in h_ref.items():
            assert( value == h_ref[key] )

    mpi.barrier()

