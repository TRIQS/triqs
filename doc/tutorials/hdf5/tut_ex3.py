from pytriqs.gf.local import GfReFreq
from pytriqs.gf.local.descriptors import SemiCircular
from pytriqs.archive import HDFArchive
import numpy

R = HDFArchive('myfile.h5', 'w') 
for D in range(1,10,2) :
    g = GfReFreq(indices = [0], window = (-2.00,2.00), name = "D=%s"%D)
    g << SemiCircular(half_bandwidth = 0.1*D)
    R[g.name]= g