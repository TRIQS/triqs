from pytriqs.arrays.block_matrix import *
from pytriqs.archive import *
import pytriqs.utility.mpi as mpi
from numpy import matrix

A=BlockMatrix(['up','down'],[matrix([[0]]), matrix([[1.0]])])
A0=mpi.bcast(A)

assert A("up")==matrix([[0.0]]), "not ok"
assert A[0]==matrix([[0.0]]), "not ok"
assert A.size() ==2, "not ok"

B=A+0.5*A+A*0.5
assert B[1]==matrix([[2.0]]), "not ok"

#HDF5
R=HDFArchive("block_matrix.output.h5",'w')
R["A"]=A
del R

R2=HDFArchive("block_matrix.output.h5",'r')
A2=R2["A"]
del R2

assert A2.matrix_vec==A.matrix_vec, "not ok" 
