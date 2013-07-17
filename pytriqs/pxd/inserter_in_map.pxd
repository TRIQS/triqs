from libcpp.string cimport string as std_string
cdef extern from "<triqs/python_tools/inserter_in_map.hpp>" namespace "triqs::python_tools" : 

    cdef cppclass inserter_in_map "triqs::python_tools::inserter_in_map" [T,U] : 
        inserter_in_map(T &)
        void operator()(std_string &, U&)
    
    cdef cppclass inserter_in_map2 "triqs::python_tools::inserter_in_map2" [T,U] : 
        inserter_in_map(T &)
        void operator()(std_string &, object)

    cdef cppclass printer "triqs::python_tools::printer" [T] : 
        printer()
        std_string operator()(T &)

