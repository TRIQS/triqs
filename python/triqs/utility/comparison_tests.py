# Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2020 Simons Foundation
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
# Authors: Alexander Hampel, Olivier Parcollet, Nils Wentzell

import numpy as np

def assert_arrays_are_close(a, b, precision = 1.e-6):
    d = np.amax(np.abs(a - b))
    assert  d< precision, "Arrays are different. Difference is %s.\n %s \n\n --------- \n\n %s"%(d,a,b)

def assert_array_close_to_scalar(a, x, precision = 1.e-6):
    assert_arrays_are_close(a, np.identity(a.shape[0])*(x), precision)

def assert_gfs_are_close(a, b, precision = 1.e-6):
    assert type(a.mesh) == type(b.mesh), "GFs have different type of meshes !"
    assert a.mesh == b.mesh, "meshes are different"
    assert_arrays_are_close(a.data, b.data, precision)

def assert_block_gfs_are_close(a, b, precision = 1.e-6):
    assert len(a) == len(b), "Block GFs have different number of blocks"
    for (nx,x),(ny,y) in zip(a,b):
        # first check if the names of the two blocks match
        assert nx == ny, "block name {} does not match {}".format(nx,ny)
        assert_gfs_are_close(x, y, precision)

def assert_block2_gfs_are_close(a, b, precision = 1.e-6):
    assert_block_gfs_are_close(a, b, precision)
