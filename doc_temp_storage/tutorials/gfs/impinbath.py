from pytriqs.gf import GfReFreq, Omega, Wilson, inverse
import numpy

eps_d,t  = 0.3, 0.2

# Create the real-frequency Green's function and initialize it
g = GfReFreq(indices = ['s','d'], window = (-2, 2), n_points = 1000, name = "$G_\mathrm{s+d}$")
g['d','d'] = Omega - eps_d
g['d','s'] = t
g['s','d'] = t
g['s','s'] = inverse( Wilson(1.0) )
g.invert()

# Plot it with matplotlib. 'S' means: spectral function ( -1/pi Imag (g) )
from pytriqs.plot.mpl_interface import oplot, plt
oplot( g['d','d'], '-o', RI = 'S', x_window  = (-1.8,1.8), name = "Impurity" )
oplot( g['s','s'], '-x', RI = 'S', x_window  = (-1.8,1.8), name = "Bath" )
plt.show()
