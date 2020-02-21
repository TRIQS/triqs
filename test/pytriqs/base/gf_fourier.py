from __future__ import unicode_literals
from pytriqs.gf import *

# ==== Matrix-valued Green functions

n_iw = 100
n_tau = 6 * n_iw + 1

gt = GfImTime(beta=20.0, n_points = n_tau, indices=[0])
gw = GfImFreq(beta=20.0, n_points = n_iw,  indices=[0])
gw2 = gw.copy()

gw << inverse(iOmega_n - SemiCircular(2.0)) + inverse(iOmega_n)
gt << Fourier(gw)

# Check that we are PH symmetric
assert abs(gt.data[0,0,0] - gt.data[-1,0,0]) < 1e-7

# Check that forward and backward transform gives Identity
gw2 << Fourier(gt)
gw3 = make_gf_from_fourier(gt)
assert abs(gw2.data[0,0,0].real) < 1e-7
assert max(abs(gw.data[:] - gw2.data[:])) < 1e-7
assert max(abs(gw.data[:] - gw3.data[:])) < 1e-7

gw << Fourier(0.3*gt)
assert abs(gw.data[0,0,0].real) < 1e-7

# ==== Scalar-valued Green functions

gt = gt[0,0]
gw = gw[0,0]
gw2 = gw2[0,0]

gw << inverse(iOmega_n - SemiCircular(2.0)) + inverse(iOmega_n)
gt << Fourier(gw)

# Check that we are PH symmetric
assert abs(gt.data[0] - gt.data[-1]) < 1e-7

# Check that forward and backward transform gives Identity
gw2 << Fourier(gt)
gw3 = make_gf_from_fourier(gt)
assert abs(gw2.data[0].real) < 1e-7
assert max(abs(gw.data[:] - gw2.data[:])) < 1e-7
assert max(abs(gw.data[:] - gw3.data[:])) < 1e-7

gw << Fourier(0.3*gt)
assert abs(gw.data[0].real) < 1e-7

# # ==== Block Green functions

blgt = BlockGf(block_list=[gt, 2 * gt])
blgw = BlockGf(block_list=[gw, 2 * gw])
blgw2 = blgw.copy()

blgw << inverse(iOmega_n - SemiCircular(2.0)) + inverse(iOmega_n)
blgt << Fourier(blgw)
set_from_fourier(blgt, blgw)
blgt << make_gf_from_fourier(blgw)

# Check that we are PH symmetric
assert abs(blgt['0'].data[0] - blgt['0'].data[-1]) < 1e-7

blgw2 << Fourier(blgt)
set_from_fourier(blgw2, blgt)
blgw3 = make_gf_from_fourier(blgt)

# Check that forward and backward transform gives Identity
for bl in ['0', '1']:
    assert abs(blgw2[bl].data[0].real) < 1e-7
    assert max(abs(blgw[bl].data[:] - blgw2[bl].data[:])) < 1e-7
    assert max(abs(blgw[bl].data[:] - blgw3[bl].data[:])) < 1e-7
