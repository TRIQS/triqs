cdef extern from "<triqs/clef.hpp>" namespace "triqs::clef" :
    ctypedef int ZERO  "0"
    ctypedef int ONE   "1"
    ctypedef int TWO   "2"
    ctypedef int THREE "3"
    ctypedef int FOUR  "4"
    ctypedef int FIVE  "5"
    ctypedef int SIX   "6"

    cdef cppclass placeholder "triqs::clef::placeholder" [T] :
        placeholder()
        #_lazy_expr operator []()

    cdef _lazy_expr ph "triqs::clef::placeholder<0>" (placeholder[ZERO] &)
    cdef _lazy_expr ph "triqs::clef::placeholder<1>" (placeholder[ONE] &)
    cdef _lazy_expr ph "triqs::clef::placeholder<2>" (placeholder[TWO] &)
    cdef _lazy_expr ph "triqs::clef::placeholder<3>" (placeholder[THREE] &)
    cdef _lazy_expr ph "triqs::clef::placeholder<4>" (placeholder[FOUR] &)
    cdef _lazy_expr ph "triqs::clef::placeholder<5>" (placeholder[FIVE] &)
    cdef _lazy_expr ph "triqs::clef::placeholder<6>" (placeholder[SIX] &)

    cdef _lazy_expr ph0 "triqs::clef::placeholder<0>" ()
    cdef _lazy_expr ph1 "triqs::clef::placeholder<1>" ()
    cdef _lazy_expr ph2 "triqs::clef::placeholder<2>" ()
    cdef _lazy_expr ph3 "triqs::clef::placeholder<3>" ()
    cdef _lazy_expr ph4 "triqs::clef::placeholder<4>" ()
    cdef _lazy_expr ph5 "triqs::clef::placeholder<5>" ()

    cdef _lazy_expr _i "triqs::clef::placeholder<0>" ()
    cdef _lazy_expr _j "triqs::clef::placeholder<1>" ()
    cdef _lazy_expr _k "triqs::clef::placeholder<2>" ()
    cdef _lazy_expr _l "triqs::clef::placeholder<3>" ()
    cdef _lazy_expr _m "triqs::clef::placeholder<4>" ()
    cdef _lazy_expr _n "triqs::clef::placeholder<5>" ()

    cdef cppclass _lazy_expr  :
        void operator << (_lazy_expr &)
        _lazy_expr operator()(_lazy_expr&)
        _lazy_expr operator()(_lazy_expr&, _lazy_expr&)
        _lazy_expr operator()(_lazy_expr&, _lazy_expr&, _lazy_expr&)
        _lazy_expr operator()(_lazy_expr&, _lazy_expr&, _lazy_expr&, _lazy_expr&)

    cdef _lazy_expr operator +(_lazy_expr,  _lazy_expr )
    cdef _lazy_expr operator -(_lazy_expr , _lazy_expr)
    cdef _lazy_expr operator *(_lazy_expr,  _lazy_expr )
    cdef _lazy_expr operator /(_lazy_expr, _lazy_expr)

    cdef _lazy_expr operator +(long,  _lazy_expr )
    cdef _lazy_expr operator +(_lazy_expr , long)
    cdef _lazy_expr operator -(long,  _lazy_expr &)
    cdef _lazy_expr operator -(_lazy_expr &, long)
    cdef _lazy_expr operator *(long,  _lazy_expr &)
    cdef _lazy_expr operator *(_lazy_expr &, long)
    cdef _lazy_expr operator /(long,  _lazy_expr &)
    cdef _lazy_expr operator /(_lazy_expr &, long)

    cdef _lazy_expr operator +(double,  _lazy_expr )
    cdef _lazy_expr operator +(_lazy_expr , double)
    cdef _lazy_expr operator -(double,  _lazy_expr &)
    cdef _lazy_expr operator -(_lazy_expr &, double)
    cdef _lazy_expr operator *(double,  _lazy_expr &)
    cdef _lazy_expr operator *(_lazy_expr &, double)
    cdef _lazy_expr operator /(double,  _lazy_expr &)
    cdef _lazy_expr operator /(_lazy_expr &, double)


