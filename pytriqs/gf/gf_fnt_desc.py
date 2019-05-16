from cpp2py.wrap_generator import *

# This modules contains functions that may be called directly by users
m = module_(full_name = "pytriqs.gf.gf_fnt", doc = "C++ wrapping of functions on Green functions ...", app_name="triqs")

m.add_imports("pytriqs.gf.meshes")

m.add_include("<triqs/gfs.hpp>")
m.add_include("<triqs/gfs/transform/pade.hpp>")
m.add_include("<triqs/gfs/legacy_for_python_api.hpp>")

m.add_include("<cpp2py/converters/pair.hpp>")
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


# ---------------------- Tail functionality --------------------
# fit_tail
m.add_function("std::pair<array<dcomplex,3>, double> fit_tail(gf_view<imfreq, matrix_valued> g, array_view<dcomplex,3> known_moments = {})", doc = "tail")
m.add_function("std::pair<array<dcomplex,1>, double> fit_tail(gf_view<imfreq, scalar_valued> g, array_view<dcomplex,1> known_moments = {})", doc = "tail")
m.add_function("std::pair<std::vector<array<dcomplex,3>>, double> fit_tail(block_gf_view<imfreq, matrix_valued> g, std::vector<array_view<dcomplex,3>> known_moments = {})", doc = "tail")
m.add_function("std::pair<std::vector<array<dcomplex,1>>, double> fit_tail(block_gf_view<imfreq, scalar_valued> g, std::vector<array_view<dcomplex,1>> known_moments = {})", doc = "tail")

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

# ---------------------- make_adjoint_mesh --------------------
m.add_function("gf_mesh<brillouin_zone> make_adjoint_mesh(gf_mesh<cyclic_lattice> m)", doc = "Create the adjoint k-mesh")
m.add_function("gf_mesh<cyclic_lattice> make_adjoint_mesh(gf_mesh<brillouin_zone> m)", doc = "Create the adjoint r-mesh")
m.add_function("gf_mesh<imfreq> make_adjoint_mesh(gf_mesh<imtime> m, int n_iw = -1)", doc = "Create the adjoint iw-mesh")
m.add_function("gf_mesh<imtime> make_adjoint_mesh(gf_mesh<imfreq> m, int n_tau = -1)", doc = "Create the adjoint tau-mesh")
m.add_function("gf_mesh<refreq> make_adjoint_mesh(gf_mesh<retime> m, bool shift_half_bin = false)", doc = "Create the adjoint w-mesh")
m.add_function("gf_mesh<refreq> make_adjoint_mesh(gf_mesh<retime> m, bool shift_half_bin = false)", doc = "Create the adjoint t-mesh")

# ---------------------- miscellaneous --------------------
for Target in  ["scalar_valued", "matrix_valued", "tensor_valued<3>", "tensor_valued<4>"]:
    for gf_type in ["gf", "block_gf", "block2_gf"]:
        gf_view_type = gf_type +  '_view'

        # make_real_in_tau
        m.add_function("%s<imfreq, %s> make_real_in_tau(%s<imfreq, %s> g)"%(gf_type, Target, gf_view_type, Target),
                    doc = "Ensures that the Fourier transform of the Gf, in tau, is real, hence G(-i \omega_n)* =G(i \omega_n)")

        # is_gf_real_in_tau
        m.add_function("bool is_gf_real_in_tau(%s<imfreq, %s> g, double tolerance = 1.e-13)"%(gf_view_type, Target))


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
m.add_function("gf<imtime, matrix_valued> rebinning_tau(gf_view<imtime,matrix_valued> g, int new_n_tau)", doc = "Rebins the data of a GfImTime on a sparser mesh")

# GfLegendre specific functions
m.add_function("void enforce_discontinuity(gf_view<triqs::gfs::legendre, matrix_valued> gl, matrix_view<double> disc)", doc = """Modify the coefficient to adjust discontinuity""")


# ---------------------- make_hermitian, is_gf_hermitian --------------------
for gf_type in ["gf", "block_gf", "block2_gf"]:
    gf_view_type = gf_type +  '_view'
    # make_hermitian
    m.add_function("%s<imfreq, scalar_valued> make_hermitian(%s<imfreq, scalar_valued> g)"%(gf_type, gf_view_type),
                doc = "Symmetrize the Green function in freq, to ensure its hermiticity (G_ij[iw] = G_ji[-iw]*)")
    m.add_function("%s<imfreq, matrix_valued> make_hermitian(%s<imfreq, matrix_valued> g)"%(gf_type, gf_view_type),
                doc = "Symmetrize the Green function in freq, to ensure its hermiticity (G_ij[iw] = G_ji[-iw]*)")

    # is_gf_hermitian
    m.add_function("bool is_gf_hermitian(%s<imfreq, scalar_valued> g, double tolerance = 1.e-13)"%gf_type)
    m.add_function("bool is_gf_hermitian(%s<imfreq, matrix_valued> g, double tolerance = 1.e-13)"%gf_type)


# ---------------------- Fourier functionality --------------------
for Target in  ["scalar_valued", "matrix_valued", "tensor_valued<3>", "tensor_valued<4>"]:

    # === Matsubara and ReTime/Freq Fourier
    for Meshes in [["imtime", "imfreq"], ["imfreq", "imtime"], ["retime", "refreq"], ["refreq", "retime"]]:
        for gf_type in ["gf", "block_gf", "block2_gf"]:
	    gf_view_type = gf_type +  '_view'

            # make_gf_from_fourier
            m.add_function(name = "make_gf_from_fourier",
                    signature="%s<%s, %s> make_gf_from_fourier(%s<%s, %s> g_in)"%(gf_type, Meshes[0], Target, gf_view_type, Meshes[1], Target),
                    doc ="""Create Green function from the Fourier transform of g_in""")

            # set_from_fourier
            m.add_function("void set_from_fourier(%s<%s, %s> g_out, %s<%s, %s> g_in)"%(gf_view_type, Meshes[0], Target, gf_view_type, Meshes[1], Target),
                    calling_pattern = "g_out = fourier(g_in)",
                    doc = """Fills self with the Fourier transform of g_in""")

        # make_gf_from_fourier with known moments
        m.add_function(name = "make_gf_from_fourier",
               signature="gf<%s, %s> make_gf_from_fourier(gf_view<%s, %s> g_in, gf_mesh<%s> mesh, array_const_view<dcomplex, %s::rank + 1> known_moments)"%(Meshes[0], Target, Meshes[1], Target, Meshes[0], Target),
               doc ="""Create Green function from the Fourier transform of g_in using the known high-frequency moments""")

        # FIXME We are making copies of the tail
        m.add_function(name = "make_gf_from_fourier",
                signature="block_gf<%s, %s> make_gf_from_fourier(block_gf_view<%s, %s> g_in, gf_mesh<%s> mesh, std::vector<array<dcomplex, %s::rank + 1>> known_moments)"%(Meshes[0], Target, Meshes[1], Target, Meshes[0], Target),
               doc ="""Create Green function from the Fourier transform of g_in using the known high-frequency moments""")

        # FIXME We are making copies of the tail
        m.add_function(name = "make_gf_from_fourier",
                signature="block2_gf<%s, %s> make_gf_from_fourier(block2_gf_view<%s, %s> g_in, gf_mesh<%s> mesh, std::vector<std::vector<array<dcomplex, %s::rank + 1>>> known_moments)"%(Meshes[0], Target, Meshes[1], Target, Meshes[0], Target),
               doc ="""Create Green function from the Fourier transform of g_in using the known high-frequency moments""")

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

    # Additional overloads for make_gf_from_fourier
    m.add_function(name = "make_gf_from_fourier",
                   signature="gf<imfreq, %s> make_gf_from_fourier(gf_view<imtime, %s> g_in, int n_iw)"%(Target, Target),
                   doc ="""Create Green function from the Fourier transform of g_tau""")
    m.add_function(name = "make_gf_from_fourier",
                   signature="gf<imtime, %s> make_gf_from_fourier(gf_view<imfreq, %s> g_in, int n_tau)"%(Target, Target),
                   doc ="""Create Green function from the Fourier transform of g_iw""")

    m.add_function(name = "make_gf_from_fourier",
                   signature="gf<refreq, %s> make_gf_from_fourier(gf_view<retime, %s> g_in, bool shift_half_bin)"%(Target, Target),
                   doc ="""Create Green function from the Fourier transform of g_t""")
    m.add_function(name = "make_gf_from_fourier",
                   signature="gf<retime, %s> make_gf_from_fourier(gf_view<refreq, %s> g_in, bool shift_half_bin)"%(Target, Target),
                   doc ="""Create Green function from the Fourier transform of g_w""")

    # === Lattice Fourier
    for Meshes in [["cyclic_lattice", "brillouin_zone"], ["brillouin_zone", "cyclic_lattice"]]:
        for gf_type in ["gf", "block_gf", "block2_gf"]:
	    gf_view_type = gf_type +  '_view'
            # make_gf_from_fourier
            m.add_function(name = "make_gf_from_fourier",
                    signature="%s<%s, %s> make_gf_from_fourier(%s<%s, %s> g_in)"%(gf_type, Meshes[0], Target, gf_view_type, Meshes[1], Target),
                    doc ="""Create Green function from the Fourier transform of g_in""")

            # set_from_fourier
            m.add_function("void set_from_fourier(%s<%s, %s> g_out, %s<%s, %s> g_in)"%(gf_view_type, Meshes[0], Target, gf_view_type, Meshes[1], Target),
                    calling_pattern = "g_out = fourier(g_in)",
                    doc = """Fills self with the Fourier transform of g_in""")


########################
##   Code generation
########################

m.generate_code()
