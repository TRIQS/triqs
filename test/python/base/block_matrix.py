# Copyright (c) 2015-2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2015-2016 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2016 Igor Krivenko
# Copyright (c) 2020 Simons Foundation
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
# Authors: Igor Krivenko, Nils Wentzell, tayral

from triqs.arrays.block_matrix import *
from h5 import *
import triqs.utility.mpi as mpi
from numpy import matrix

#####################
# Real block matrix #
#####################

m0 = matrix([[1,0],[0,2]])
m1 = matrix([[0,1],[2,0]])

A = BlockMatrix(['up','dn'],[m0,m1])

# General
assert (A["up"] == m0).all(), "block name lookup failed"
assert (A(1) == m1).all(), "indexing failed"
assert A.size() == 2, "size() failed"
assert len(A) == 2, "__len__() failed"

assert BlockMatrix().block_names == [] and BlockMatrix().matrix_vec == [], "empty construction failed"

# Arithmetics
A2 = A + A
assert (A2["up"] == 2*m0).all() and (A2["dn"] == 2*m1).all(), "Addition failed"
A2 += A
assert (A2["up"] == 3*m0).all() and (A2["dn"] == 3*m1).all(), "In-place addition failed"
A2 = A - A
assert (A2["up"] == 0*m0).all() and (A2["dn"] == 0*m1).all(), "Subtraction failed"
A2 -= A
assert (A2["up"] == -m0).all() and (A2["dn"] == -m1).all(), "In-place subtraction failed"
A2 = A * A
assert (A2["up"] == m0*m0).all() and (A2["dn"] == m1*m1).all(), "Multiplication failed"
A2 *= A
assert (A2["up"] == m0*m0*m0).all() and (A2["dn"] == m1*m1*m1).all(), "In-place multiplication failed"
A2 = 2*A
print("A ", A)
assert (A2["up"] == 2*m0).all() and (A2["dn"] == 2*m1).all(), "Multiplication by constant failed"
A2 = A*2
assert (A2["up"] == 2*m0).all() and (A2["dn"] == 2*m1).all(), "Multiplication by constant failed"
A2 = A/2
assert (A2["up"] == 0.5*m0).all() and (A2["dn"] == 0.5*m1).all(), "Division by constant failed"
A2 = -A
assert (A2["up"] == -m0).all() and (A2["dn"] == -m1).all(), "Unary minus failed"

# MPI
Abcast = mpi.bcast(A)
assert (A['up'] == Abcast['up']).all() and (A['dn'] == Abcast['dn']).all(), "mpi.bcast failed"

# HDF5
with HDFArchive("block_matrix.output.h5",'w') as R: R["A"] = A
with HDFArchive("block_matrix.output.h5",'r') as R: A2 = R["A"]
assert (A['up'] == A2['up']).all() and (A['dn'] == A2['dn']).all(), "HDF5 write/read failed"


########################
# Complex block matrix #
########################

m0 = matrix([[1,0],[0,2j]])
m1 = matrix([[0,1j],[2,0]])

A = BlockMatrixComplex(['up','dn'],[m0,m1])

# General
assert (A["up"] == m0).all(), "block name lookup failed"
assert (A(1) == m1).all(), "indexing failed"
assert A.size() == 2, "size() failed"
assert len(A) == 2, "__len__() failed"

assert BlockMatrixComplex().block_names == [] and BlockMatrixComplex().matrix_vec == [], "empty construction failed"

# Arithmetics
A2 = A + A
assert (A2["up"] == 2*m0).all() and (A2["dn"] == 2*m1).all(), "Addition failed"
A2 += A
assert (A2["up"] == 3*m0).all() and (A2["dn"] == 3*m1).all(), "In-place addition failed"
A2 = A - A
assert (A2["up"] == 0*m0).all() and (A2["dn"] == 0*m1).all(), "Subtraction failed"
A2 -= A
assert (A2["up"] == -m0).all() and (A2["dn"] == -m1).all(), "In-place subtraction failed"
A2 = A * A
assert (A2["up"] == m0*m0).all() and (A2["dn"] == m1*m1).all(), "Multiplication failed"
A2 *= A
assert (A2["up"] == m0*m0*m0).all() and (A2["dn"] == m1*m1*m1).all(), "In-place multiplication failed"
A2 = 2*A
assert (A2["up"] == 2*m0).all() and (A2["dn"] == 2*m1).all(), "Multiplication by constant failed"
A2 = A*2
assert (A2["up"] == 2*m0).all() and (A2["dn"] == 2*m1).all(), "Multiplication by constant failed"
A2 = A/2
assert (A2["up"] == 0.5*m0).all() and (A2["dn"] == 0.5*m1).all(), "Division by constant failed"
A2 = 2j*A
assert (A2["up"] == 2j*m0).all() and (A2["dn"] == 2j*m1).all(), "Multiplication by complex constant failed"
A2 = A*2j
assert (A2["up"] == 2j*m0).all() and (A2["dn"] == 2j*m1).all(), "Multiplication by complex constant failed"
A2 = A/2j
assert (A2["up"] == -0.5j*m0).all() and (A2["dn"] == -0.5j*m1).all(), "Division by complex constant failed"
A2 = -A
assert (A2["up"] == -m0).all() and (A2["dn"] == -m1).all(), "Unary minus failed"

# MPI
Abcast = mpi.bcast(A)
assert (A['up'] == Abcast['up']).all() and (A['dn'] == Abcast['dn']).all(), "mpi.bcast failed"

# HDF5
with HDFArchive("block_matrix.output.h5",'w') as R: R["A"] = A
with HDFArchive("block_matrix.output.h5",'r') as R: A2 = R["A"]
assert (A['up'] == A2['up']).all() and (A['dn'] == A2['dn']).all(), "HDF5 write/read failed"

