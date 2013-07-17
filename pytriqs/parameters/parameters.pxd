from libcpp.string cimport string as std_string
from libcpp.vector cimport vector
cdef extern from "triqs/parameters/parameters.hpp" namespace "triqs::parameters" : 
    
    cdef cppclass _object "triqs::utility::_object":
    #cdef cppclass _object "triqs::parameters::_object":
        _object()

    #cdef cppclass parameters " triqs::parameters::parameters" :
    cdef cppclass parameters "triqs::utility::parameters" :
        parameters()
        _object operator[](std_string &)
        std_string print_ "print" ()

    cdef cppclass parameter_defaults "triqs::utility::parameter_defaults" :
        parameter_defaults()
        vector[vector[std_string]] generate_help()


cdef class Parameters:
    cdef parameters _c
