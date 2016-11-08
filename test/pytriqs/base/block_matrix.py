from pytriqs.arrays.block_matrix import *
from pytriqs.archive import *
import pytriqs.utility.mpi as mpi
from numpy import matrix

A=BlockMatrix(['up','down'],[matrix([[0]]), matrix([[1.0]])])
A0=mpi.bcast(A)

assert A["up"]==matrix([[0.0]]), "not ok"
assert A(0)==matrix([[0.0]]), "not ok"
assert A.size() ==2, "not ok"

B=A+0.5*A+A*0.5
assert B(1)==matrix([[2.0]]), "not ok"

# HDF5
with HDFArchive("block_matrix.output.h5",'w') as R: R["A"] = A
with HDFArchive("block_matrix.output.h5",'r') as R: A2 = R["A"]

assert A2.matrix_vec==A.matrix_vec, "not ok"

A3 = BlockMatrixComplex(['up','down'],[matrix([[0]]), matrix([[1.0*1j]])])

with HDFArchive("block_matrix.output.h5",'a') as R: R['A3'] = A3
with HDFArchive("block_matrix.output.h5",'r') as R: A4 = R['A3']

assert A3.matrix_vec==A4.matrix_vec, "not ok"
