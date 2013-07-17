#!python
cimport cython  

cdef extern from "./bug.hpp" : 
    cdef cppclass Obj1 :
        Obj1()

cdef class G_generic : 
    def __init__(self) : 
        pass 
    
cdef class G_cython (G_generic) : 
    cdef Obj1 co
    def __init__(self, A):
        G_generic.__init__(self)
        self.co = Obj1() 

    # Comment this and there is a memory leak
    def __dealloc__(self) : 
        pass

class G (G_cython) : 
    def __init__(self, A): 
        G_cython.__init__(self,A)



