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
c.add_method("int convert_index(std::string s, int i)", doc = "index -> position conversion")
c.add_method_copy()
c.add_getitem(signature = "std::vector<std::string> operator[](int d)", doc = "The list of indices for dimension d")

c.add_len(calling_pattern = "int result = self_c.rank()", doc = "")
c.add_iterator(c_cast_type = "std::vector<std::string>")

m.add_class (c)


########################

# FIXME
# New tail
m.add_function("std::pair<array<dcomplex,3>, double> fit_tail(gf_view<imfreq, matrix_valued> g, array_view<dcomplex,3> known_moments = {})", doc = "tail")
m.add_function("std::pair<array<dcomplex,1>, double> fit_tail(gf_view<imfreq, scalar_valued> g, array_view<dcomplex,1> known_moments = {})", doc = "tail")

# density
m.add_function("matrix<dcomplex> density(gf_view<imfreq, matrix_valued> g, array_view<dcomplex, 3> known_moments = {})",   doc = "Density, as a matrix, computed from a Matsubara sum")
m.add_function("dcomplex density(gf_view<imfreq, scalar_valued> g, array_view<dcomplex, 1> known_moments = {})",   doc = "Density, as a complex, computed from a Matsubara sum")

m.add_function("matrix<dcomplex> density(gf_view<legendre, matrix_valued> g)", doc = "Density, as a matrix, computed from a Matsubara sum")
m.add_function("dcomplex density(gf_view<legendre, scalar_valued> g)", doc = "Density, as a complex, computed from a Matsubara sum")

for Target in  ["scalar_valued", "matrix_valued", "tensor_valued<3>", "tensor_valued<4>"]:


    for Meshes in [["imtime", "imfreq"], ["retime", "refreq"]]: # Fourier with known moments

            # set_from_fourier with known moments
            m.add_function("void set_from_fourier(gf_view<%s, %s> g_out, gf_view<%s, %s> g_in, array_const_view<dcomplex, 1+%s::rank> moments)"%(Meshes[1], Target, Meshes[0], Target, Target),
                           calling_pattern = "g_out = fourier(g_in, moments)",
                           doc = """Fills self with the Fourier transform of g_in with possible known moments""")

            # set_from_fourier with known moments
            m.add_function("void set_from_inverse_fourier(gf_view<%s, %s> g_out, gf_view<%s, %s> g_in, array_const_view<dcomplex, 1+%s::rank> moments)"%(Meshes[0], Target, Meshes[1], Target, Target),
                           calling_pattern = "g_out = fourier(g_in, moments)",
                           doc = """Fills self with the Fourier transform of g_in with possible known moments""")


    for Meshes in [["imtime", "imfreq"], ["retime", "refreq"], ["cyclic_lattice", "brillouin_zone"]]:

        for gf_type in ["gf_view", "block_gf_view", "block2_gf_view"]:

            # Setter direct
            m.add_function("void set_from_fourier(%s<%s, %s> g_out, %s<%s, %s> g_in)"%(gf_type, Meshes[1], Target, gf_type, Meshes[0], Target),
                    calling_pattern = "g_out = fourier(g_in)",
                    doc = """Fills self with the Fourier transform of g_in""")

            # Setter inverse
            m.add_function("void set_from_inverse_fourier(%s<%s, %s> g_out, %s<%s, %s> g_in)"%(gf_type, Meshes[0], Target, gf_type, Meshes[1], Target),
                    calling_pattern = "g_out = fourier(g_in)",
                    doc = """Fills self with the inverse Fourier transform of g_in""")
            m.add_function("void set_from_fourier(%s<%s, %s> g_out, %s<%s, %s> g_in)"%(gf_type, Meshes[0], Target, gf_type, Meshes[1], Target),
                    calling_pattern = "g_out = fourier(g_in)",
                    doc = """Fills self with the Fourier transform of g_in""")

            # Factory function direct fourier
            m.add_function(name = "make_gf_from_fourier",
                    signature="%s<%s, %s> make_gf_from_fourier(%s<%s, %s> g_in)"%(gf_type, Meshes[1], Target, gf_type, Meshes[0], Target),
                    doc ="""Create Green function from the Fourier transform of g_in""")

            # Factory function inverse fourier
            m.add_function(name = "make_gf_from_inverse_fourier",
                    signature="%s<%s, %s> make_gf_from_fourier(%s<%s, %s> g_in)"%(gf_type, Meshes[0], Target, gf_type, Meshes[1], Target),
                    doc ="""Create Green function from the inverse Fourier transform of g_in""")

            m.add_function(name = "make_gf_from_fourier",
                    signature="%s<%s, %s> make_gf_from_fourier(%s<%s, %s> g_in)"%(gf_type, Meshes[0], Target, gf_type, Meshes[1], Target),
                    doc ="""Create Green function from the Fourier transform of g_in""")

    # === Specific overloads for make_gf_from_fourier
    m.add_function(name = "make_gf_from_fourier",
                   signature="gf_view<imfreq, %s> make_gf_from_fourier(gf_view<imtime, %s> g_in, int n_iw)"%(Target, Target),
                   doc ="""Create Green function from the Fourier transform of g_tau""")
    m.add_function(name = "make_gf_from_fourier",
                   signature="gf_view<imtime, %s> make_gf_from_fourier(gf_view<imfreq, %s> g_in, int n_tau)"%(Target, Target),
                   doc ="""Create Green function from the Fourier transform of g_iw""")

    m.add_function(name = "make_gf_from_fourier",
                   signature="gf_view<refreq, %s> make_gf_from_fourier(gf_view<retime, %s> g_in, bool shift_half_bin)"%(Target, Target),
                   doc ="""Create Green function from the Fourier transform of g_t""")
    m.add_function(name = "make_gf_from_fourier",
                   signature="gf_view<retime, %s> make_gf_from_fourier(gf_view<refreq, %s> g_in, bool shift_half_bin)"%(Target, Target),
                   doc ="""Create Green function from the Fourier transform of g_w""")

    # make_real_in_tau
    m.add_function("gf_view<imfreq, %s> make_real_in_tau(gf_view<imfreq, %s> g)"%(Target, Target),
                doc = "Ensures that the Fourier transform of the Gf, in tau, is real, hence G(-i \omega_n)* =G(i \omega_n)")

    # is_gf_real_in_tau
    m.add_function("bool is_gf_real_in_tau(gf_view<imfreq, %s> g, double tolerance = 1.e-13)"%Target)


# set_from_legendre
m.add_function("void set_from_legendre(gf_view<imfreq, matrix_valued> gw, gf_view<legendre, matrix_valued> gl)",
            calling_pattern = "gw = legendre_to_imfreq(gl)",
            doc = """Fills self with the legendre transform of gl""")

m.add_function("void set_from_legendre(gf_view<imtime, matrix_valued> gt, gf_view<legendre, matrix_valued> gl)",
            calling_pattern = "gt = legendre_to_imtime(gl)",
            doc = """Fills self with the legendre transform of gl""")

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
m.add_function("void set_from_pade (gf_view<refreq, scalar_valued> gw, gf_view<imfreq, scalar_valued> giw, int n_points = 100, double freq_offset = 0.0)",
             calling_pattern = "pade(gw, giw, n_points, freq_offset)",
             doc = """""")

# make_real_in_tau
m.add_function("gf_view<imfreq, matrix_valued> make_real_in_tau(gf_view<imfreq, matrix_valued> g)",
               doc = "Ensures that the Fourier transform of the Gf, in tau, is real, hence G(-i \omega_n)* =G(i \omega_n)")

# fit_tail
m.add_function("std::pair<array<dcomplex,3>, double> fit_tail_on_window(gf_const_view<imfreq, matrix_valued> g,  int n_min, int n_max, array_const_view<dcomplex, 3> known_moments, int n_tail_max, int expansion_order)", 
                doc = """Fits the tail on the [n_min, n_max] window  + negative counter part""")

m.add_function("void replace_by_tail(gf_view<imfreq, matrix_valued> g, array_const_view<dcomplex, 3> tail, int n_min)", 
                doc = """Replace the function by the evaluation of the tail for |n| > n_min""")
m.add_function("void replace_by_tail_in_fit_window(gf_view<imfreq, matrix_valued> g, array_const_view<dcomplex, 3> tail)", 
                doc = """Replace the function by the evaluation of the tail for |n| > n_min""")
  
# GfImTime specific functions

# rebinning_tau
m.add_function("gf<imtime, matrix_valued> rebinning_tau(gf_view<imtime,matrix_valued> g, int new_n_tau)", doc = "Rebins the data of a GfImTime on a sparser mesh")

# GfLegendre specific functions
m.add_function("void enforce_discontinuity(gf_view<legendre, matrix_valued> gl, matrix_view<double> disc)", doc = """Modify the coefficient to adjust discontinuity""")

########################
##   Code generation
########################

m.generate_code()

