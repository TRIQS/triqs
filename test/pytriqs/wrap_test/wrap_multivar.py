from test_multivar import *
from pytriqs.archive import HDFArchive
from pytriqs.utility.comparison_tests import *

gamma1 = make_vertex(1.0)
pass_vertex(gamma1+gamma1)

gamma2 = make_vertex(2.0)

assert_arrays_are_close((gamma1+gamma1).data, gamma2.data, 1e-10)
assert_arrays_are_close(HDFArchive("make_vertex.h5",'r')['gamma'].data,
                        HDFArchive("pass_vertex.h5",'r')['gamma'].data,
                        1e-10)

block_gamma1 = make_block_vertex(1.0)
pass_block_vertex(block_gamma1+block_gamma1)

gamma_mv = HDFArchive("make_block_vertex.h5",'r')['gamma']
gamma_pv = HDFArchive("pass_block_vertex.h5",'r')['gamma']

assert_arrays_are_close(2*gamma_mv['up','up'].data, gamma_pv['up','up'].data, 1e-10)
assert_arrays_are_close(2*gamma_mv['up','dn'].data, gamma_pv['up','dn'].data, 1e-10)
assert_arrays_are_close(2*gamma_mv['dn','up'].data, gamma_pv['dn','up'].data, 1e-10)
assert_arrays_are_close(2*gamma_mv['dn','dn'].data, gamma_pv['dn','dn'].data, 1e-10)
