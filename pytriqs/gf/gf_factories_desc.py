from cpp2py.wrap_generator import *

# This modules contains factory functions that generate a Green function
m = module_(full_name = "pytriqs.gf.gf_factories", doc = "C++ wrapping of functions on Green functions ...", app_name="triqs")

m.add_imports("pytriqs.gf.meshes")

m.add_include("<triqs/gfs.hpp>")
m.add_include("<triqs/gfs/legacy_for_python_api.hpp>")

m.add_include("<cpp2py/converters/pair.hpp>")
m.add_include("<cpp2py/converters/vector.hpp>")
m.add_include("<triqs/cpp2py_converters.hpp>")

m.add_using("namespace triqs::arrays")
m.add_using("namespace triqs::gfs")

# ---------------------- make_hermitian, make_real_in_tau --------------------
for gf_type in ["gf", "block_gf", "block2_gf"]:
    gf_view_type = gf_type +  '_view'
    # make_hermitian
    m.add_function("%s<imfreq, scalar_valued> make_hermitian(%s<imfreq, scalar_valued> g)"%(gf_type, gf_view_type),
                doc = "Symmetrize the Green function in freq, to ensure its hermiticity (G_ij[iw] = G_ji[-iw]*)")
    m.add_function("%s<imfreq, matrix_valued> make_hermitian(%s<imfreq, matrix_valued> g)"%(gf_type, gf_view_type),
                doc = "Symmetrize the Green function in freq, to ensure its hermiticity (G_ij[iw] = G_ji[-iw]*)")

    # make_real_in_tau
    m.add_function("%s<imfreq, scalar_valued> make_real_in_tau(%s<imfreq, scalar_valued> g)"%(gf_type, gf_view_type),
                doc = "Symmetrize the Green function in freq, to ensure its hermiticity (G_ij[iw] = G_ji[-iw]*)")
    m.add_function("%s<imfreq, matrix_valued> make_real_in_tau(%s<imfreq, matrix_valued> g)"%(gf_type, gf_view_type),
                doc = "Symmetrize the Green function in freq, to ensure its hermiticity (G_ij[iw] = G_ji[-iw]*)")


# ---------------------- make_gf_from_fourier --------------------
for Target in  ["scalar_valued", "matrix_valued", "tensor_valued<3>", "tensor_valued<4>"]:

    # === Matsubara and ReTime/Freq Fourier
    for Meshes in [["imtime", "imfreq"], ["imfreq", "imtime"], ["retime", "refreq"], ["refreq", "retime"]]:
        for gf_type in ["gf", "block_gf", "block2_gf"]:
	    gf_view_type = gf_type +  '_view'

            # make_gf_from_fourier
            m.add_function(name = "make_gf_from_fourier",
                    signature="%s<%s, %s> make_gf_from_fourier(%s<%s, %s> g_in)"%(gf_type, Meshes[0], Target, gf_view_type, Meshes[1], Target),
                    doc ="""Create Green function from the Fourier transform of g_in""")

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

########################
##   Code generation
########################

m.generate_code()
