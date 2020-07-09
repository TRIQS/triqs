from triqs.gf import GfReFreq
from h5 import HDFArchive
from math import pi

R = HDFArchive('myfile.h5', 'r') 
 
from triqs.plot.mpl_interface import oplot, plt

for name, g in list(R.items()) :  # iterate on the elements of R, like a dict ...
    oplot( (- 1/pi * g).imag, "-o", name = name)

plt.xlim(-1,1) 
plt.ylim(0,7) 

p.savefig("./tut_ex3b.png") 

