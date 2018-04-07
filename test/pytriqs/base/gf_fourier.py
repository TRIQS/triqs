from pytriqs.gf import *

# ==== Matrix-valued Green functions

n_iw = 100
n_tau = 2 * n_iw + 1

gt = GfImTime(beta=20.0, n_points = n_tau, indices=[0])
gw = GfImFreq(beta=20.0, n_points = n_iw,  indices=[0])
gw2 = gw.copy()

gw << inverse(iOmega_n - SemiCircular(2.0)) + inverse(iOmega_n)
gt << InverseFourier(gw)

print abs(gt.data[0,0,0] - gt.data[-1,0,0])
assert abs(gt.data[0,0,0] - gt.data[-1,0,0]) < 1e-10

gw2 << Fourier(gt)
gw3 = make_gf_from_fourier(gt)
assert abs(gw2.data[0,0,0].real) < 1e-10
assert max(abs(gw.data[:] - gw2.data[:])) < 1e-10
assert max(abs(gw.data[:] - gw3.data[:])) < 1e-10

gw << Fourier(0.3*gt)
assert abs(gw.data[0,0,0].real) < 1e-10

# ==== Scalar-valued Green functions

gt = gt[0,0]
gw = gw[0,0]
gw2 = gw2[0,0]

gw << inverse(iOmega_n - SemiCircular(2.0)) + inverse(iOmega_n)
gt << InverseFourier(gw)

assert abs(gt.data[0] - gt.data[-1]) < 1e-10

gw2 << Fourier(gt)
gw3 = make_gf_from_fourier(gt)
assert abs(gw2.data[0].real) < 1e-10
assert max(abs(gw.data[:] - gw2.data[:])) < 1e-10
assert max(abs(gw.data[:] - gw3.data[:])) < 1e-10

gw << Fourier(0.3*gt)
assert abs(gw.data[0].real) < 1e-10

# ==== Block Green functions

blgt = BlockGf(name_list=[0,1], block_list=[gt, gt])
blgw = BlockGf(name_list=[0,1], block_list=[gw, gw])
blgw2 = blgw.copy()

blgw << inverse(iOmega_n - SemiCircular(2.0)) + inverse(iOmega_n)
blgt << InverseFourier(blgw)
blgt << make_gf_from_inverse_fourier(blgw)

assert abs(blgt[0].data[0] - blgt[0].data[-1]) < 1e-10

blgw2 << Fourier(blgt)
blgw3 = make_gf_from_fourier(blgt)

assert abs(blgw2.data[0].real) < 1e-10
assert max(abs(blgw.data[:] - blgw2.data[:])) < 1e-10
assert max(abs(blgw.data[:] - blgw3.data[:])) < 1e-10
