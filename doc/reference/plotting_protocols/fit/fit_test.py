from pytriqs.plot.mpl_interface import oplot
from pytriqs.gf.local import GfImFreq, Omega, inverse

g = GfImFreq(indices = [0], beta = 300, n_points = 1000, name = "g")
g <<= inverse( Omega + 0.5 )

# the data we want to fit...
# The green function for omega \in [0,0.2]
X,Y = g.x_data_view (x_window = (0,0.2), flatten_y = True )

from pytriqs.fit import Fit, linear, quadratic

fitl = Fit ( X,Y.imag, linear )
fitq = Fit ( X,Y.imag, quadratic )

oplot (g,     '-o', x_window = (0,5) )     
oplot (fitl , '-x', x_window = (0,0.5) )
oplot (fitq , '-x', x_window = (0,1) )

# a bit more complex, we want to fit with a one fermion level ....
# Cf the definition of linear and quadratic in the lib
one_fermion_level  =  lambda X, a,b   : 1/(a * X *1j  + b),    r"${1}/(%f x + %f)$"    , (1,1)

fit1 = Fit ( X,Y, one_fermion_level )
oplot (fit1 , '-x', x_window = (0,3) )
    
