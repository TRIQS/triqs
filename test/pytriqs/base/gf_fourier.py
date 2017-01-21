from pytriqs.gf import *

gt = GfImTime(beta=20.0, indices=[0])
gw = GfImFreq(beta=20.0, indices=[0])

gw << inverse(iOmega_n - SemiCircular(2.0)) + inverse(iOmega_n)
gt << InverseFourier(gw)

assert abs(gt.data[0,0,0] - gt.data[-1,0,0]) < 1e-12

gw << Fourier(gt)
assert abs(gw.data[0,0,0].real) < 1e-12

# buggy: remove the tail in gt ...
#gw << Fourier(gt*gt*gt)
#R =  gw.data[:,0,0].real
#assert abs(gw.data[0,0,0].real) < 1e-12

gw << Fourier(0.3*gt)
assert abs(gw.data[0,0,0].real) < 1e-12

