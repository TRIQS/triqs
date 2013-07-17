cdef extern from "<memory>" namespace "std" : 
    cdef cppclass shared_ptr "std::shared_ptr" [T] : 
        shared_ptr(T*)
        shared_ptr()
        T* get()
        void reset (T*)

 
