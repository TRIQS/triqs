# Copyright (c) 2020 Simons Foundation
# Copyright (c) 2016 Igor Krivenko
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You may obtain a copy of the License at
#     https:#www.gnu.org/licenses/gpl-3.0.txt
#
# Authors: Igor Krivenko, Nils Wentzell

from test_multivar import *
from h5 import HDFArchive
from triqs.utility.comparison_tests import *

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
