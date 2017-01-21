# Import the Green's functions
from pytriqs.gf import GfImFreq, iOmega_n, inverse

# Create the Matsubara-frequency Green's function and initialize it
g = GfImFreq(indices = [1], beta = 50, n_points = 1000, name = "imp")
g << inverse( iOmega_n + 0.5 )

import pytriqs.utility.mpi as mpi

mpi.bcast(g)



#Block

from pytriqs.gf import *
g1 = GfImFreq(indices = ['eg1','eg2'], beta = 50, n_points = 1000, name = "egBlock")
g2 = GfImFreq(indices = ['t2g1','t2g2','t2g3'], beta = 50, n_points = 1000, name = "t2gBlock")
G = BlockGf(name_list = ('eg','t2g'), block_list = (g1,g2), make_copies = False)


mpi.bcast(G)


#imtime 
from pytriqs.gf import *

# A Green's function on the Matsubara axis set to a semicircular
gw = GfImFreq(indices = [1], beta = 50)
gw << SemiCircular(half_bandwidth = 1)

# Create an imaginary-time Green's function and plot it
gt = GfImTime(indices = [1], beta = 50)
gt << InverseFourier(gw)

mpi.bcast(gt)


##

gt2 = GfImTime(indices = [1], beta = 50)

gt2 = mpi.bcast(gt2)

print gt2.tail.order_max
print gt2.tail.order_min

gw2 = GfImFreq(indices = [1], beta = 50)
gw2 << Fourier(gt2)


