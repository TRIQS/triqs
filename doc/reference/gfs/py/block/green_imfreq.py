from pytriqs.gf.local import GfImFreq, SemiCircular

g = GfImFreq(indices = ['eg1','eg2'], beta = 50, n_points = 1000, name = "egBlock")

g['eg1','eg1'] = SemiCircular(half_bandwidth = 1)
g['eg2','eg2'] = SemiCircular(half_bandwidth = 2)

from pytriqs.plot.mpl_interface import oplot
oplot(g, '-o', x_window = (0,10))
plt.ylim(-2,1)

