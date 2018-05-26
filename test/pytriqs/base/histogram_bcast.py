
""" Construct a random histogram, broadcast it and compare
the broadcasted result with a reference. """

import numpy as np
import pytriqs.utility.mpi as mpi
from pytriqs.statistics.histograms import Histogram

def py_histogram(int_vec):
    
    hist = {}
    for val in int_vec:
        hist[val] = 1 + hist.get(val, 0)

    return hist

if mpi.is_master_node():

    x = np.array([9, 4, 5, 2, 6, 2, 8, 2, 0, 2, 7, 7, 0, 8, 4, 1, 5, 7, 4, 8, 2, 8,
       0, 2, 5, 0, 2, 1, 8, 7, 1, 8, 8, 1, 0, 1, 0, 3, 8, 4, 9, 2, 1, 7,
       4, 4, 1, 5, 1, 2, 3, 5, 1, 8, 6, 3, 7, 7, 0, 0, 1, 6, 3, 5, 7, 6,
       2, 8, 2, 0, 7, 0, 2, 9, 2, 6, 7, 0, 1, 8, 1, 9, 5, 2, 0, 0, 0, 0,
       1, 7, 5, 5, 1, 1, 5, 6, 8, 4, 5, 1, 0, 2, 1, 1, 9, 1, 7, 1, 3, 7,
       0, 6, 7, 6, 6, 3, 5, 3, 3, 9, 7, 8, 3, 0, 1, 7, 8, 2, 4, 4, 7, 3,
       1, 2, 1, 1, 8, 9, 8, 7, 2, 6, 8, 4, 3, 9, 3, 4, 2, 6, 6, 4, 6, 5,
       1, 6, 6, 6, 5, 2, 1, 5, 8, 2, 6, 0, 6, 7, 5, 1, 4, 2, 3, 8, 8, 9,
       3, 6, 0, 5, 0, 4, 2, 2, 1, 1, 3, 3, 6, 7, 6, 5, 6, 8, 7, 7, 5, 9,
       7, 2, 1, 2, 3, 7, 6, 4, 7, 0, 6, 7, 0, 3, 6, 1, 2, 7, 0, 3, 9, 4,
       9, 0, 1, 3, 6, 6, 9, 0, 2, 5, 3, 0, 7, 4, 0, 6, 1, 7, 4, 8, 7, 7,
       5, 2, 9, 6, 0, 7, 6, 8, 2, 7, 1, 2, 8, 5, 5, 1, 9, 3, 6, 6, 6, 0,
       4, 5, 1, 9, 8, 0, 4, 5, 3, 2, 4, 8, 8, 6, 4, 3, 5, 3, 9, 8, 1, 0,
       4, 2, 2, 0, 5, 8, 6, 6, 2, 3, 5, 3, 8, 5, 2, 3, 4, 3, 4, 1, 0, 4,
       0, 0, 8, 4, 3, 5, 8, 4, 5, 3, 5, 4, 7, 7, 0, 2, 5, 9, 0, 9, 2, 8,
       0, 4, 2, 2, 0, 2, 3, 0, 9, 6, 9, 7, 2, 4, 9, 7, 0, 3, 8, 0, 0, 7,
       8, 7, 6, 4, 0, 3, 7, 1, 9, 2, 8, 1, 2, 6, 4, 8, 9, 6, 2, 9, 9, 2,
       8, 4, 1, 0, 7, 5, 9, 0, 4, 8, 0, 0, 4, 3, 7, 5, 7, 7, 5, 0, 5, 8,
       8, 7, 2, 7])

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
        pts = np.array([ int(h.mesh_point(idx)) for idx in xrange(len(h))])

        for pt, val in zip(pts, h.data):
            val = int(val)
            #print pt, val
            if val > 0:
                assert( val == h_ref[pt] )
            else:
                assert( not h_ref.has_key(pt) )
        
    mpi.barrier()

