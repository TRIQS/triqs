from clef cimport *
cdef extern from "<triqs/python_tools/array_interface.hpp>" namespace "triqs::arrays" :

    cdef cppclass array "triqs::arrays::array" [T,R] : 
        array()
        array(object)  except +
        array operator +( array_view &) 
        array operator -( array_view &) 
        array operator *( array_view &) 
        array operator /( array_view &) 
        object to_python()

    cdef cppclass array_view "triqs::arrays::array_c" [T,R] : 
        array_view() 
        array_view(object) except +
        array_view operator +( array_view &) 
        array_view operator -( array_view &) 
        array_view operator *( array_view &) 
        array_view operator /( array_view &) 
        void operator << (array_view &)
        object to_python()
        int shape(int)
        # bug in cython
        #_lazy_expr operator()(_lazy_expr&)
        #_lazy_expr operator()(_lazy_expr&, _lazy_expr&)
        #_lazy_expr operator()(_lazy_expr&, _lazy_expr&, _lazy_expr&)
        #_lazy_expr operator()(_lazy_expr&, _lazy_expr&, _lazy_expr&, _lazy_expr&)
        _lazy_expr call "operator()" (_lazy_expr&)
        _lazy_expr call "operator()" (_lazy_expr&, _lazy_expr&)
        _lazy_expr call "operator()" (_lazy_expr&, _lazy_expr&, _lazy_expr&)
        _lazy_expr call "operator()" (_lazy_expr&, _lazy_expr&, _lazy_expr&, _lazy_expr&)
        _lazy_expr call "operator()" (_lazy_expr&, _lazy_expr&, _lazy_expr&, _lazy_expr&, _lazy_expr&)

    cdef cppclass matrix_view "triqs::arrays::matrix_c" [T] :
        matrix_view() 
        matrix_view(matrix) 
        matrix_view(object)  except +
        void operator << (matrix_view &)
        object to_python()

    cdef cppclass matrix "triqs::arrays::matrix" [T] :
        matrix()
        matrix(object)  except +
        matrix operator +( matrix_view &)
        matrix operator -( matrix_view &)
        matrix operator /( matrix_view &)
        object to_python()
        
    cdef matrix_view operator +( matrix_view & , matrix_view &)
    cdef matrix_view operator -( matrix_view & , matrix_view &)
    cdef matrix_view operator *( matrix_view & , matrix_view &)
    cdef matrix_view operator /( matrix_view & , matrix_view &)
 
    cdef cppclass tqa_vector "triqs::arrays::vector" [T] :
        tqa_vector()
        tqa_vector(object)  except +
        tqa_vector operator +( tqa_vector_view &)
        tqa_vector operator -( tqa_vector_view &)
        tqa_vector operator *( tqa_vector_view &)
        tqa_vector operator /( tqa_vector_view &)
        object to_python()

    cdef cppclass tqa_vector_view "triqs::arrays::vector_c" [T] :
        tqa_vector_view() 
        tqa_vector_view(object) except +
        tqa_vector_view operator +(tqa_vector_view &)
        tqa_vector_view operator -(tqa_vector_view &)
        tqa_vector_view operator *(tqa_vector_view &)
        tqa_vector_view operator /(tqa_vector_view &)
        void operator << (tqa_vector_view &)
        object to_python()

