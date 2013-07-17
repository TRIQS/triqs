#!python
#cython: embedsignature=True
from cython.operator cimport dereference as deref, preincrement as inc #dereference and increment operators
#cimport cython
import numpy
import string
import warnings

from arrays cimport *

def f(A) :

    # Solution 1
    cdef array_view[long,TWO] a = array_view[long,TWO](A)
    # ....
    a.call ( _i(), _j()) << ( _i() + 3*_j() + 2 )
    # can not do this ... unless with preprocessing the cython files...
    # indeed _i is placeholder[ONE], not lazy_expr, I need to erase its type for
    # cython compiler...
    #a.call ( _i, _j) << ( _i + 3*_j + 2 )
    #....
    return A

    #Solution 2
    cdef placeholder[ONE] i_
    cdef placeholder[TWO] j_
    array_view[long,TWO] (A).call ( ph(i_), ph(j_)) << ph(i_) + ph(j_)

    #Solution 3 ?
    array_view[long,TWO] (A).call ( ph0(), ph1()) << ph0() + ph1()

    #array_view[long,TWO] (A).call ( _i(), _j()) << 3*_i() + _j()*2

    #array_view[long,TWO] (A)( ph(i_), ph(j_)) << ph(i_) + ph(j_)

    return A



