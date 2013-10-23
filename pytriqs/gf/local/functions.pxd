###############  Density  #########################

cdef extern from "triqs/gfs/local/functions.hpp":
    matrix_view density(gf_imfreq &)
    matrix_view density(gf_legendre &)
    void enforce_discontinuity(gf_legendre &, array_view[double,TWO])

###############  Fourier  #########################

cdef extern from "triqs/gfs/local/fourier_matsubara.hpp" : 
    gf_imfreq fourier          (gf_imtime & )
    gf_imtime inverse_fourier  (gf_imfreq & )

cdef extern from "triqs/gfs/local/fourier_real.hpp" :
    gf_refreq fourier          (gf_retime & )
    gf_retime inverse_fourier  (gf_refreq & ) 
    #gf_refreq fourier               (gf_retime & ) except +
    #gf_retime inverse_fourier       (gf_refreq & ) except +

###############  Legendre  #########################

cdef extern from "triqs/gfs/local/legendre_matsubara.hpp" : 
    gf_imfreq legendre_to_imfreq    (gf_legendre &) 
    gf_imtime legendre_to_imtime    (gf_legendre &)
    gf_legendre imfreq_to_legendre  (gf_imfreq &) 
    gf_legendre imtime_to_legendre  (gf_imtime &) 

###############  Pade  #########################

cdef extern from "triqs/gfs/local/pade.hpp":
    void pade(gf_refreq &, gf_imfreq &, int, double) except +
