from pytriqs.gf.local import *
from pytriqs.gf.local.descriptors import Omega
g = GfImFreq(indices = [1], beta = 50, n_points = 1000, name = "g")
g <<= inverse( Omega + 0.5 )

# open 2 panels top (t) and bottom (b) 
from pytriqs.plot.mpl_interface import subplots
f, (t,b) = subplots( 2,1)

#plot ...
t.oplot(g.real, '-o', x_window = (0,10) )
b.oplot(g.imag, '-x', x_window = (0,12) )   
b.oplot( lambda om : -om*0.8/(om*om + 4), name = "Bad Fit !")
b.text(5,-0.5, r'$g(i\omega_n) = \frac{1}{i \omega_n + 1/2} $', size = 20, color='r')

