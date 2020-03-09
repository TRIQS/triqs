from pytriqs.plot.mpl_interface import oplot
from pytriqs.fit.fit import Fit, linear, quadratic
from pytriqs.gf import *
from pytriqs.gf.descriptors import iOmega_n
g = GfImFreq(indices = [1], beta = 300, n_points = 1000, name = "g")
g << inverse( iOmega_n + 0.5 )

print(" van plot")
oplot (g,     '-o', x_window = (0,3) )     

print("plot done")
g << inverse( iOmega_n + 0.5 )
 
print("ok ----------------------")


from pytriqs.archive import HDFArchive
R = HDFArchive('myfile.h5', 'r')

for n, calculation in R.items() : 
    #g = calculation['g']
    g << inverse( iOmega_n + 0.5 )
    
    print("pokokook")

    X,Y = g.x_data_view (x_window = (0,0.2), flatten_y = True )

    #fitl = Fit ( X,Y.imag, linear )
    g << inverse( iOmega_n + 0.5 )

    print(" van plot")
    oplot (g,     '-o', x_window = (0,3) )     
    g << inverse( iOmega_n + 0.5 )
    
    print(g)
    g << inverse( iOmega_n + 0.5 )
    break
    ginv =  inverse( g )
    print(ginv)
    sigma = iOmega_n - g
    #oplot (ginv,     '-o', x_window = (0,3) )     
    
    #break
    oplot (sigma,     '-o', x_window = (0,3) )     
    #oplot (fitl , '-x', x_window = (0,0.5) )

    #print n, fitl.param[0]
