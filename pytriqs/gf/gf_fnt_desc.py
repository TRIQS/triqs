from cpp2py.wrap_generator import *
import re

# This modules contains functions that may be called directly by users
m = module_(full_name = "pytriqs.gf.gf_fnt", doc = "C++ wrapping of functions on Green functions ...", app_name="triqs")

import meshes

m.add_include("<triqs/gfs.hpp>")
m.add_include("<triqs/gfs/transform/pade.hpp>")
m.add_include("<triqs/gfs/legacy_for_python_api.hpp>")

m.add_include("<cpp2py/converters/vector.hpp>")
m.add_include("<triqs/cpp2py_converters.hpp>")

m.add_using("namespace triqs::arrays")
m.add_using("namespace triqs::gfs")
m.add_using("triqs::utility::mini_vector")
m.add_preamble("""
""")

########################
##   Indices
########################

# Wrapping indices
c = class_( py_type = "GfIndices",
        c_type = "gf_indices",
        c_type_absolute = "triqs::gfs::gf_indices",
        hdf5 = True,
        serializable= "tuple",
       )

c.add_constructor("(std::vector<std::vector<std::string>> indices)", doc = "Constructs indices, from a list(list(string)))")
c.add_property(getter = cfunction("std::vector<std::vector<std::string>> data()"), doc = "Get a copy of the list of list of strings")
c.add_method("gf_indices transpose()", doc = "Transposing")
c.add_method("triqs::arrays::range convert_index(std::string s, int i)", doc = "index -> position conversion")
c.add_method_copy()
c.add_getitem(signature = "std::vector<std::string> operator[](int d)", doc = "The list of indices for dimension d")

c.add_len(calling_pattern = "int result = self_c.rank()", doc = "")
c.add_iterator(c_cast_type = "std::vector<std::string>")

m.add_class (c)


########################

# FIXME
# New tail
m.add_function("std::pair<array<dcomplex,3>, double> fit_tail(gf_view<imfreq, matrix_valued> g)", doc = "tail")
m.add_function("std::pair<array<dcomplex,3>, double> fit_tail(gf_view<imfreq, matrix_valued> g, array_view<dcomplex,3> known_moments)", doc = "tail")

# density
m.add_function("matrix<dcomplex> density(gf_view<imfreq, matrix_valued> g)",   doc = "Density, as a matrix, computed from a Matsubara sum")
m.add_function("matrix<dcomplex> density(gf_view<legendre, matrix_valued> g)", doc = "Density, as a matrix, computed from a Matsubara sum")

# enforce_discontinuity
m.add_function("void enforce_discontinuity(gf_view<legendre, matrix_valued> gl, matrix_view<double> disc)", doc = """Modify the coefficient to adjust discontinuity""")

# set_from_fourier
m.add_function("void set_from_fourier(gf_view<imfreq, matrix_valued> gw, gf_view<imtime, matrix_valued> gt, array_const_view<dcomplex, 3> moments = {})",
               calling_pattern = "gw = fourier(gt, moments)",
               doc = """Fills self with the Fourier transform of gt""")

m.add_function("void set_from_fourier(gf_view<refreq, matrix_valued> gw, gf_view<retime, matrix_valued> gt)",
             calling_pattern = "gw = fourier(gt)",
             doc = """Fills self with the Fourier transform of gt""")

m.add_function("void set_from_fourier(gf_view<brillouin_zone, matrix_valued> gk, gf_view<cyclic_lattice, matrix_valued> gr)",
               calling_pattern = "gk = fourier(gr)",
               doc = """Fills self with the Fourier transform of gr""")

# set_from_inverse_fourier
m.add_function("void set_from_inverse_fourier(gf_view<imtime, matrix_valued> gt, gf_view<imfreq, matrix_valued> gw)",
               calling_pattern = "gt = fourier(gw)",
               doc = """Fills self with the Inverse Fourier transform of gw""")

m.add_function("void set_from_inverse_fourier(gf_view<retime, matrix_valued> gt,  gf_view<refreq, matrix_valued> gw)",
             calling_pattern = "gt = fourier(gw)",
             doc = """Fills self with the Inverse Fourier transform of gw""")

m.add_function("void set_from_inverse_fourier(gf_view<cyclic_lattice, matrix_valued> gr, gf_view<brillouin_zone, matrix_valued> gk)",
               calling_pattern = "gr = fourier(gk)",
               doc = """Fills self with the Fourier transform of gk""")

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
m.add_function(name = "make_gf_from_inverse_fourier", signature="gf_view<retime, matrix_valued> make_gf_from_fourier(gf_view<refreq, matrix_valued> gw)", doc ="")

# make_real_in_tau
m.add_function("gf_view<imfreq, matrix_valued> make_real_in_tau(gf_view<imfreq, matrix_valued> g)",
               doc = "Ensures that the Fourier transform of the Gf, in tau, is real, hence G(-i \omega_n)* =G(i \omega_n)")

# fit_tail
m.add_function("std::pair<array<dcomplex,3>, double> fit_tail_on_window(gf_const_view<imfreq, matrix_valued> g,  int n_min, int n_max, array_const_view<dcomplex, 3> known_moments, int n_tail_max, int expansion_order)", 
                doc = """Fits the tail on the [n_min, n_max] window  + negative counter part""")

m.add_function("void replace_by_tail(gf_view<imfreq, matrix_valued> g, array_const_view<dcomplex, 3> tail, int n_min)", 
                doc = """Replace the function by the evaluation of the tail for |n| > n_min""")
  
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

