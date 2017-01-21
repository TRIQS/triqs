from wrap_generator import *
import re

# This modules contains functions that may be called directly by users
m = module_(full_name = "pytriqs.gf.gf_fnt", doc = "C++ wrapping of functions on Green functions ...", app_name="triqs")
m.use_module('lattice_tools')
m.use_module('meshes')
m.use_module('singularities')

m.add_include("<triqs/gfs/transform/pade.hpp>")
m.add_include("<triqs/gfs/legacy_for_python_api.hpp>")
m.add_using("namespace triqs::arrays")
m.add_using("namespace triqs::gfs")
m.add_using("triqs::utility::mini_vector")
m.add_preamble("""
""")

# density
m.add_function("matrix<dcomplex> density(gf_view<imfreq, matrix_valued> g)",   doc = "Density, as a matrix, computed from a Matsubara sum")
m.add_function("matrix<dcomplex> density(gf_view<legendre, matrix_valued> g)", doc = "Density, as a matrix, computed from a Matsubara sum")

# enforce_discontinuity
m.add_function("void enforce_discontinuity(gf_view<legendre, matrix_valued> gl, matrix_view<double> disc)", doc = """Modify the coefficient to adjust discontinuity""")

# set_from_fourier
m.add_function("void set_from_fourier(gf_view<imfreq, matrix_valued> gw, gf_view<imtime, matrix_valued> gt)",
               calling_pattern = "gw = fourier(gt)",
               doc = """Fills self with the Fourier transform of gt""")

m.add_function("void set_from_fourier(gf_view<refreq, matrix_valued> gw, gf_view<retime, matrix_valued> gt)",
             calling_pattern = "gw = fourier(gt)",
             doc = """Fills self with the Fourier transform of gt""")

# set_from_inverse_fourier
m.add_function("void set_from_inverse_fourier(gf_view<imtime, matrix_valued> gt, gf_view<imfreq, matrix_valued> gw)",
               calling_pattern = "gt = inverse_fourier(gw)",
               doc = """Fills self with the Inverse Fourier transform of gw""")

m.add_function("void set_from_inverse_fourier(gf_view<retime, matrix_valued> gt,  gf_view<refreq, matrix_valued> gw)",
             calling_pattern = "gt = inverse_fourier(gw)",
             doc = """Fills self with the Inverse Fourier transform of gw""")

# set_from_legendre
m.add_function("void set_from_legendre(gf_view<imfreq, matrix_valued> gw, gf_view<legendre, matrix_valued> gl)",
               calling_pattern = "gw = legendre_to_imfreq(gl)",
               doc = """Fills self with the legendre transform of gl""")

m.add_function("void set_from_legendre(gf_view<imtime, matrix_valued> gt, gf_view<legendre, matrix_valued> gl)",
               calling_pattern = "gt = legendre_to_imtime(gl)",
               doc = """Fills self with the legendre transform of gl""")

# replace imfreq_to_legendre !!
# set_from_imfreq
m.add_function("void set_from_imfreq(gf_view<legendre, matrix_valued> gl, gf_view<imfreq, matrix_valued> gw)", 
               calling_pattern = "gl = imfreq_to_legendre(gw)",
               doc = """Fills self with the legendre transform of gw""")

# set_from_imtime 
m.add_function("void set_from_imtime(gf_view<legendre, matrix_valued> gl, gf_view<imtime, matrix_valued> gt)",
             calling_pattern = "gl = imtime_to_legendre(gt)",
             doc = """Fills self with the legendre transform of gt""")

# set_from_imfreq
m.add_function("void set_from_imfreq(gf_view<legendre, matrix_valued> gl, gf_view<imfreq, matrix_valued> gw)",
             calling_pattern = "gl = imfreq_to_legendre(gw)",
             doc = """Fills self with the legendre transform of gw""")

# set_from_pade
m.add_function("void set_from_pade (gf_view<refreq, matrix_valued> gw, gf_view<imfreq, matrix_valued> giw, int n_points = 100, double freq_offset = 0.0)",
             calling_pattern = "pade(gw, giw, n_points, freq_offset)",
             doc = """""")

# make_gf_from_inverse_fourier
m.add_function(name = "make_gf_from_inverse_fourier", signature="gf_view<retime, matrix_valued> make_gf_from_inverse_fourier(gf_view<refreq, matrix_valued> gw)", doc ="")

# make_real_in_tau
m.add_function("gf_view<imfreq, matrix_valued> make_real_in_tau(gf_view<imfreq, matrix_valued> g)",
               doc = "Ensures that the Fourier transform of the Gf, in tau, is real, hence G(-i \omega_n)* =G(i \omega_n)")

# fit_tail
m.add_function("void fit_tail(gf_view<imfreq, matrix_valued> g, tail_view known_moments, int max_moment, int n_min, int n_max, bool replace_by_fit = true)", 
                doc = """Set the tail by fitting""")
m.add_function("void fit_tail(gf_view<imfreq, matrix_valued> g, tail_view known_moments, int max_moment, int neg_n_min, int neg_n_max, int pos_n_min, int pos_n_max, bool replace_by_fit = true)", 
                doc = """Set the tail by fitting""")
  
# GfImTime specific functions

# rebinning_tau
m.add_function("gf<imtime, matrix_valued> rebinning_tau(gf_view<imtime,matrix_valued> g, int new_n_tau)", doc = "Rebins the data of a GfImTime on a sparser mesh")

# is_gf_real_in_tau
m.add_function("bool is_gf_real_in_tau(gf_view<imfreq, matrix_valued> g, double tolerance = 1.e-13)")

# GfLegendre specific functions 
m.add_function("void enforce_discontinuity(gf_view<legendre, matrix_valued> gl, matrix_view<double> disc)", doc = """Modify the coefficient to adjust discontinuity""")

########################
##   Code generation
########################

m.generate_code()

