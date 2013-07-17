cdef extern from * :
    object PyCObject_FromVoidPtr(void* cobj, void *)
    void* PyCObject_AsVoidPtr(object self)

cdef inline encapsulate (void * p) :
    return PyCObject_FromVoidPtr(p, NULL)

cdef extern from "triqs/utility/python/extractor.hpp"  : 

    cdef cppclass extractor "triqs::utility::python::extractor" [T] :
        extractor (object)
        T & operator()()

