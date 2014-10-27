from pytriqs.gf.local import *
from pytriqs.archive import *
from pytriqs.plot.mpl_interface import oplot

A = HDFArchive("solution.h5")
G_iw = GfImFreq(indices = [0], beta = 50.0)
G_iw <<= Fourier(A['G_tau']['up'])
oplot(G_iw, '-o', x_window = (0,10))
