from triqs.random_generator import *
from triqs.plot.mpl_interface import *

r = RandomGenerator("lagged_fibonacci607", 237489)
l = []
for i in range(10000): l += [r(),]
plt.hist(l, 30, density=True)
