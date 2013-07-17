cdef extern from "triqs/python_tools/boost_object.hpp" namespace "triqs::python_tools":

  cdef cppclass boost_object "triqs::python_tools::boost_object":
    boost_object(object ob)

