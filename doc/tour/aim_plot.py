from pytriqs.gf.local import *
from pytriqs.archive import *
from pytriqs.plot.mpl_interface import oplot

A = HDFArchive("solution.h5")
oplot(A['G']['up'], '-o', x_window = (0,10))
