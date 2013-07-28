from dcomplex cimport * 
from arrays cimport *   
cdef extern from "triqs/gf/local/tail.hpp" : 
    cdef cppclass tail "triqs::python_tools::cython_proxy<triqs::gfs::local::tail_view>"  :
        tail()
        tail(array_view[dcomplex,THREE], int, array_view[long,TWO]) except +
        matrix_view[dcomplex] operator()(int) except +
        array_view[dcomplex,THREE] data()
        array_view[long,TWO] mask_view() except +
        void operator << (tail &) except +
        void operator << (dcomplex &) except +
        long order_min() except +
        long order_max() except +
        size_t size()  except +
        size_t shape(int) except +

    cdef tail operator +( tail &, tail &) except + 
    cdef tail operator -( tail &, tail &) except + 
    
    cdef tail operator *( tail&, tail &) except +

    cdef tail operator *( double, tail &) except + 
    cdef tail operator *( tail &, double) except + 
    cdef tail operator /( double, tail &) except + 
    cdef tail operator /( tail &, double) except + 

    cdef tail operator *( dcomplex, tail &) except + 
    cdef tail operator *( tail &, dcomplex) except + 
    cdef tail operator /( dcomplex, tail &) except + 
    cdef tail operator /( tail &, dcomplex) except + 
    cdef tail inverse_c "inverse" ( tail &) except +

    cdef void h5_write (h5_group, char *, tail &)

cdef extern from "triqs/utility/serialization.hpp"  :
    cdef std_string boost_serialize "triqs::serialize" (tail &)
    cdef void boost_unserialize_into "triqs::deserialize_into_view" (std_string, tail &)
