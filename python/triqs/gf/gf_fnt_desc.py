from cpp2py.wrap_generator import *

# This modules contains functions that take a Green function as a first argument
# They are automatically registered as member functions of the Gf class in the triqs/gf/gf.py file
m = module_(full_name = "triqs.gf.gf_fnt", doc = "C++ wrapping of functions on Green functions ...", app_name="triqs")

m.add_imports("triqs.gf.meshes")

m.add_include("<triqs/gfs.hpp>")
m.add_include("<triqs/gfs/transform/pade.hpp>")
m.add_include("<triqs/gfs/legacy_for_python_api.hpp>")

m.add_include("<cpp2py/converters/pair.hpp>")
m.add_include("<cpp2py/converters/vector.hpp>")
m.add_include("<triqs/cpp2py_converters.hpp>")

m.add_using("namespace triqs::arrays")
m.add_using("namespace triqs::gfs")
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


# ---------------------- Tail functionality --------------------
# fit_tail
for mesh in ["imfreq", "refreq"]:
    m.add_function("std::pair<array<dcomplex,3>, double> fit_tail(gf_view<%s, matrix_valued> g, array_view<dcomplex,3> known_moments = {})" % mesh, doc = "tail")
    m.add_function("std::pair<array<dcomplex,1>, double> fit_tail(gf_view<%s, scalar_valued> g, array_view<dcomplex,1> known_moments = {})" % mesh, doc = "tail")
    m.add_function("std::pair<std::vector<array<dcomplex,3>>, double> fit_tail(block_gf_view<%s, matrix_valued> g, std::vector<array_view<dcomplex,3>> known_moments = {})" % mesh, doc = "tail")
    m.add_function("std::pair<std::vector<array<dcomplex,1>>, double> fit_tail(block_gf_view<%s, scalar_valued> g, std::vector<array_view<dcomplex,1>> known_moments = {})" % mesh, doc = "tail")

# fit_hermitian_tail: impose hermiticity on the moment matrices
m.add_function("std::pair<array<dcomplex,3>, double> fit_hermitian_tail(gf_view<imfreq, matrix_valued> g, array_view<dcomplex,3> known_moments = {})", doc = "tail")
m.add_function("std::pair<array<dcomplex,1>, double> fit_hermitian_tail(gf_view<imfreq, scalar_valued> g, array_view<dcomplex,1> known_moments = {})", doc = "tail")
m.add_function("std::pair<std::vector<array<dcomplex,3>>, double> fit_hermitian_tail(block_gf_view<imfreq, matrix_valued> g, std::vector<array_view<dcomplex,3>> known_moments = {})", doc = "tail")
m.add_function("std::pair<std::vector<array<dcomplex,1>>, double> fit_hermitian_tail(block_gf_view<imfreq, scalar_valued> g, std::vector<array_view<dcomplex,1>> known_moments = {})", doc = "tail")

# fit_tail_on_window, replace_by_tail
m.add_function("std::pair<array<dcomplex,3>, double> fit_tail_on_window(gf_const_view<imfreq, matrix_valued> g,  int n_min, int n_max, array_const_view<dcomplex, 3> known_moments, int n_tail_max, int expansion_order)",
                doc = """Fits the tail on the [n_min, n_max] window  + negative counter part""")
m.add_function("std::pair<array<dcomplex,3>, double> fit_hermitian_tail_on_window(gf_const_view<imfreq, matrix_valued> g,  int n_min, int n_max, array_const_view<dcomplex, 3> known_moments, int n_tail_max, int expansion_order)",
                doc = """Fits the tail on the [n_min, n_max] window  + negative counter part with the contraint of hermitian moment matrices""")
m.add_function("void replace_by_tail(gf_view<imfreq, matrix_valued> g, array_const_view<dcomplex, 3> tail, int n_min)",
                doc = """Replace the function by the evaluation of the tail for |n| > n_min""")
m.add_function("void replace_by_tail_in_fit_window(gf_view<imfreq, matrix_valued> g, array_const_view<dcomplex, 3> tail)",
                doc = """Replace the function by the evaluation of the tail for |n| > n_min""")

# ---------------------- Density functions --------------------
m.add_function("matrix<dcomplex> density(gf_view<imfreq, matrix_valued> g, array_view<dcomplex, 3> known_moments = {})",   doc = "Density, as a matrix, computed from a Matsubara sum")
m.add_function("dcomplex density(gf_view<imfreq, scalar_valued> g, array_view<dcomplex, 1> known_moments = {})",   doc = "Density, as a complex, computed from a Matsubara sum")

m.add_function("matrix<dcomplex> density(gf_view<refreq, matrix_valued> g)", doc = "Density, as a matrix, computed from a frequency integral at zero temperature")
m.add_function("dcomplex density(gf_view<refreq, scalar_valued> g)", doc = "Density, as a complex, computed from a  frequency integral at zero temperature")

m.add_function("matrix<dcomplex> density(gf_view<refreq, matrix_valued> g, double beta)", doc = "Density, as a matrix, computed from a frequency integral at finite temperature")
m.add_function("dcomplex density(gf_view<refreq, scalar_valued> g, double beta)", doc = "Density, as a complex, computed from a  frequency integral at finite temperature")

m.add_function("matrix<dcomplex> density(gf_view<triqs::gfs::legendre, matrix_valued> g)", doc = "Density, as a matrix, computed from evaluation in imaginary time")
m.add_function("dcomplex density(gf_view<triqs::gfs::legendre, scalar_valued> g)", doc = "Density, as a complex, computed from evaluation in imaginary time")

# ---------------------- miscellaneous --------------------
for Target in  ["scalar_valued", "matrix_valued", "tensor_valued<3>", "tensor_valued<4>"]:

    # set_from_legendre
    m.add_function("void set_from_legendre(gf_view<imfreq, %s> gw, gf_view<triqs::gfs::legendre, %s> gl)"%(Target, Target),
                calling_pattern = "gw = legendre_to_imfreq(gl)",
                doc = """Fills self with the legendre transform of gl""")

    m.add_function("void set_from_legendre(gf_view<imtime, %s> gt, gf_view<triqs::gfs::legendre, %s> gl)"%(Target, Target),
                calling_pattern = "gt = legendre_to_imtime(gl)",
                doc = """Fills self with the legendre transform of gl""")

    # set_from_imfreq
    m.add_function("void set_from_imfreq(gf_view<triqs::gfs::legendre, %s> gl, gf_view<imfreq, %s> gw)"%(Target, Target),
                calling_pattern = "gl = imfreq_to_legendre(gw)",
                doc = """Fills self with the legendre transform of gw""")

    # set_from_imtime
    m.add_function("void set_from_imtime(gf_view<triqs::gfs::legendre, %s> gl, gf_view<imtime, %s> gt)"%(Target, Target),
                calling_pattern = "gl = imtime_to_legendre(gt)",
                doc = """Fills self with the legendre transform of gt""")

    # set_from_pade
    m.add_function("void set_from_pade (gf_view<refreq, matrix_valued> gw, gf_view<imfreq, matrix_valued> giw, int n_points = 100, double freq_offset = 0.0)",
                calling_pattern = "pade(gw, giw, n_points, freq_offset)",
                doc = """""")

# rebinning_tau
m.add_function("gf<imtime, matrix_valued> rebinning_tau(gf_view<imtime,matrix_valued> g, size_t new_n_tau)", doc = "Rebins the data of a GfImTime on a sparser mesh")

# GfLegendre specific functions
m.add_function("void enforce_discontinuity(gf_view<triqs::gfs::legendre, matrix_valued> gl, matrix_view<double> disc)", doc = """Modify the coefficient to adjust discontinuity""")


# ---------------------- is_gf_hermitian, is_real_in_tau --------------------
for gf_type in ["gf", "block_gf", "block2_gf"]:
    gf_view_type = gf_type +  '_view'

    # is_gf_hermitian imfreq
    m.add_function("bool is_gf_hermitian(%s<imfreq, scalar_valued> g, double tolerance = 1.e-12)"%gf_type)
    m.add_function("bool is_gf_hermitian(%s<imfreq, matrix_valued> g, double tolerance = 1.e-12)"%gf_type)
    m.add_function("bool is_gf_hermitian(%s<imfreq, tensor_valued<4>> g, double tolerance = 1.e-12)"%gf_type)

    # is_gf_hermitian imtime
    m.add_function("bool is_gf_hermitian(%s<imtime, scalar_valued> g, double tolerance = 1.e-12)"%gf_type)
    m.add_function("bool is_gf_hermitian(%s<imtime, matrix_valued> g, double tolerance = 1.e-12)"%gf_type)
    m.add_function("bool is_gf_hermitian(%s<imtime, tensor_valued<4>> g, double tolerance = 1.e-12)"%gf_type)

    # is_gf_real_in_tau
    m.add_function("bool is_gf_real_in_tau(%s<imfreq, scalar_valued> g, double tolerance = 1.e-12)"%gf_type)
    m.add_function("bool is_gf_real_in_tau(%s<imfreq, matrix_valued> g, double tolerance = 1.e-12)"%gf_type)


# ---------------------- Fourier functionality --------------------
for Target in  ["scalar_valued", "matrix_valued", "tensor_valued<3>", "tensor_valued<4>"]:

    # === Matsubara and ReTime/Freq Fourier
    for Meshes in [["imtime", "imfreq"], ["imfreq", "imtime"], ["retime", "refreq"], ["refreq", "retime"]]:
        for gf_type in ["gf", "block_gf", "block2_gf"]:
            gf_view_type = gf_type +  '_view'

            # set_from_fourier
            m.add_function("void set_from_fourier(%s<%s, %s> g_out, %s<%s, %s> g_in)"%(gf_view_type, Meshes[0], Target, gf_view_type, Meshes[1], Target),
                    calling_pattern = "g_out = fourier(g_in)",
                    doc = """Fills self with the Fourier transform of g_in""")

            if Meshes[1] in ["imtime", "retime"]:
                m.add_function("void set_from_fourier(%s<%s, %s> g_out, %s<%s, typename %s::real_t> g_in)"%(gf_view_type, Meshes[0], Target, gf_view_type, Meshes[1], Target),
                        calling_pattern = "g_out = fourier(g_in)",
                        doc = """Fills self with the Fourier transform of g_in""")

        # set_from_fourier with known moments
        m.add_function("void set_from_fourier(gf_view<%s, %s> g_out, gf_view<%s, %s> g_in, array_const_view<dcomplex, 1+%s::rank> known_moments)"%(Meshes[0], Target, Meshes[1], Target, Target),
                calling_pattern = "g_out = fourier(g_in, known_moments)",
                doc = """Fills self with the Fourier transform of g_in with known high-frequency moments""")

        ## FIXME No yet implemented in C++
        ## FIXME We are making copies of the tail
        #m.add_function("void set_from_fourier(block_gf_view<%s, %s> g_out, block_gf_view<%s, %s> g_in, std::vector<array<dcomplex, 1+%s::rank>> known_moments)"%(Meshes[0], Target, Meshes[1], Target, Target),
        #        calling_pattern = "g_out = fourier(g_in, known_moments)",
        #        doc = """Fills self with the Fourier transform of g_in with known high-frequency moments""")

        ## FIXME We are making copies of the tail
        #m.add_function("void set_from_fourier(block2_gf_view<%s, %s> g_out, block2_gf_view<%s, %s> g_in, std::vector<std::vector<array<dcomplex, 1+%s::rank>>> known_moments)"%(Meshes[0], Target, Meshes[1], Target, Target),
        #        calling_pattern = "g_out = fourier(g_in, known_moments)",
        #        doc = """Fills self with the Fourier transform of g_in with known high-frequency moments""")

    # === Lattice Fourier
    for Meshes in [["cyclat", "brzone"], ["brzone", "cyclat"]]:
        for gf_type in ["gf", "block_gf", "block2_gf"]:
            gf_view_type = gf_type +  '_view'
            # set_from_fourier
            m.add_function("void set_from_fourier(%s<%s, %s> g_out, %s<%s, %s> g_in)"%(gf_view_type, Meshes[0], Target, gf_view_type, Meshes[1], Target),
                    calling_pattern = "g_out = fourier(g_in)",
                    doc = """Fills self with the Fourier transform of g_in""")


########################
##   Code generation
########################

m.generate_code()
