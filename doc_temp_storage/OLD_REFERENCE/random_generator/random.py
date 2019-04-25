from pytriqs.random_generator import *
from pytriqs.plot.mpl_interface import *

r = RandomGenerator("lagged_fibonacci607", 237489)
l = []
for i in range(10000): l += [r(),]
plt.hist(l, 30, normed=True)
