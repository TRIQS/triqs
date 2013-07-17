#!python
#cython: embedsignature=True
from cython.operator cimport dereference as deref, preincrement as inc #dereference and increment operators
cimport cython  
import numpy
import string
import warnings

from dcomplex cimport * 
from arrays cimport *   
#from libcpp.vector cimport vector
#from libcpp.string cimport string as std_string
#from extractor cimport *

cdef extern from "./f.hpp" : 
    cdef void  f_c2 "f2" ( array_view[long,TWO] & )
    cdef void  f_c "f" ( array_view[long,THREE] & )

def f(A) :
   print "----   calling  f-------------"
   print " A[:,:,0] = \n", A[:,:,0] 
   print " A[:,:,1] = \n", A[:,:,1] 
   f_c(array_view[long,THREE](A))
   #f_c(array_view[long,TWO](A))
   print "----   END calling  f-------------"
  


