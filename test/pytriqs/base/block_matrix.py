from pytriqs.arrays.block_matrix import *
from pytriqs.archive import *
import pytriqs.utility.mpi as mpi
from numpy import matrix

A=BlockMatrix(['up'],matrix([[0]]))
A0=mpi.bcast(A)

R=HDFArchive("block_matrix.output.h5",'w')
R["A"]=A
del R
