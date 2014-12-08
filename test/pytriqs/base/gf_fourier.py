from pytriqs.gf.local import *

gt = GfImTime(beta=20.0, indices=[0])
gw = GfImFreq(beta=20.0, indices=[0])

gw << inverse(iOmega_n - SemiCircular(2.0)) + inverse(iOmega_n)
gt << InverseFourier(gw)

if abs(gt.data[0,0,0] - gt.data[-1,0,0]) < 1e-12: print "Inverse Fourier fine"

gw << Fourier(gt)
if abs(gw.data[0,0,0].real) < 1e-12: print "Fourier of gt fine"

gw << Fourier(gt*gt*gt)
if abs(gw.data[0,0,0].real) < 1e-12: print "Fourier of gt*gt*gt fine"

gw << Fourier(0.3*gt)
if abs(gw.data[0,0,0].real) < 1e-12: print "Fourier of 0.3*gt fine"

