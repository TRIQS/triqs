import numpy as np
from triqs.gf import GfReFreq, SemiCircular

g = GfReFreq(indices = ['eg1', 'eg2'], window = (-5, 5), n_points = 1000, name = "egBlock")

g['eg1','eg1'] = SemiCircular(half_bandwidth = 1)
g['eg2','eg2'] = SemiCircular(half_bandwidth = 2)

from triqs.plot.mpl_interface import oplot
oplot(g['eg1','eg1'], '-o', RI = 'S')  # S : spectral function 
oplot(g['eg2','eg2'], '-x', RI = 'S')


