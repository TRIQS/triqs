cdef extern from "<complex>" namespace "std":
    cdef cppclass dcomplex "std::complex<double>":
         dcomplex()
         dcomplex(dcomplex &)
         dcomplex(double,double)
         double real()
         double imag()

# Python -> C
cdef inline dcomplex as_dcomplex (a) : 
    x = complex(a)
    return dcomplex(a.real, a.imag)

# C -> Python 
cdef inline make_dcomplex (dcomplex z) :
    return complex(z.real(), z.imag())

# Python -> C
cdef inline float as_float (a) : 
    return float (a) 

