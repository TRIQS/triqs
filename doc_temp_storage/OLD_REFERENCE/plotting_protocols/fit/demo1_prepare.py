import numpy as np
from pytriqs.plot.mpl_interface import oplot
from pytriqs.gf import *
from pytriqs.gf.descriptors import iOmega_n
g = GfImFreq(indices = [1], beta = 300, n_points = 1000, name = "g")

from pytriqs.archive import HDFArchive
R = HDFArchive('myfile.h5', 'w')

for n, Z0 in enumerate( np.arange (1,0, -0.1) ) :
    g << inverse( iOmega_n + 0.5  - iOmega_n * ( 1 - 1/Z0) ) # / (1 + 4*iOmega_n*iOmega_n) )
    g.name = "Z = %s"%Z0 
    R[ str(n) ] = { 'Z0' : Z0, 'g' : g} 

