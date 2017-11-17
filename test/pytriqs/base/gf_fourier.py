from pytriqs.gf import *

# ==== Matrix-valued Green functions

gt = GfImTime(beta=20.0, indices=[0])
gw = GfImFreq(beta=20.0, indices=[0])

gw << inverse(iOmega_n - SemiCircular(2.0)) + inverse(iOmega_n)
gt << InverseFourier(gw)

print abs(gt.data[0,0,0] - gt.data[-1,0,0])
assert abs(gt.data[0,0,0] - gt.data[-1,0,0]) < 1e-10

gw << Fourier(gt)
assert abs(gw.data[0,0,0].real) < 1e-10

gw << Fourier(0.3*gt)
assert abs(gw.data[0,0,0].real) < 1e-10

# ==== Scalar-valued Green functions

gt = gt[0,0]
gw = gw[0,0]

gw << inverse(iOmega_n - SemiCircular(2.0)) + inverse(iOmega_n)
gt << InverseFourier(gw)

assert abs(gt.data[0] - gt.data[-1]) < 1e-10

gw << Fourier(gt)
assert abs(gw.data[0].real) < 1e-10

gw << Fourier(0.3*gt)
assert abs(gw.data[0].real) < 1e-10
