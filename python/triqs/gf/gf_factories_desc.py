from cpp2py.wrap_generator import *

# This modules contains factory functions that generate a Green function
m = module_(full_name = "triqs.gf.gf_factories", doc = "C++ wrapping of functions on Green functions ...", app_name="triqs")

m.add_imports("triqs.gf.meshes")

m.add_include("<triqs/gfs.hpp>")
m.add_include("<triqs/gfs/legacy_for_python_api.hpp>")

m.add_include("<cpp2py/converters/pair.hpp>")
m.add_include("<cpp2py/converters/vector.hpp>")
m.add_include("<triqs/cpp2py_converters.hpp>")

m.add_using("namespace triqs::arrays")
m.add_using("namespace triqs::gfs")
m.add_using("namespace triqs::mesh")

# ---------------------- make_hermitian, make_real_in_tau --------------------
for gf_type in ["gf", "block_gf", "block2_gf"]:
    gf_view_type = gf_type +  '_view'
    # make_hermitian imfreq
    m.add_function("%s<imfreq, scalar_valued> make_hermitian(%s<imfreq, scalar_valued> g)"%(gf_type, gf_view_type),
                doc = "Symmetrize the Green function in freq, to ensure its hermiticity (G[iw] = G[-iw]*)")
    m.add_function("%s<imfreq, matrix_valued> make_hermitian(%s<imfreq, matrix_valued> g)"%(gf_type, gf_view_type),
                doc = "Symmetrize the Green function in freq, to ensure its hermiticity (G_ij[iw] = G_ji[-iw]*)")
    m.add_function("%s<imfreq, tensor_valued<4>> make_hermitian(%s<imfreq, tensor_valued<4>> g)"%(gf_type, gf_view_type),
                doc = "Symmetrize the Green function in freq, to ensure its hermiticity (G_ijkl[iw] = G_klij[-iw]*)")

    # make_hermitian imtime
    m.add_function("%s<imtime, scalar_valued> make_hermitian(%s<imtime, scalar_valued> g)"%(gf_type, gf_view_type),
                doc = "Symmetrize the Green function in imaginary time, to ensure its hermiticity (G[tau] = G[tau]*)")
    m.add_function("%s<imtime, matrix_valued> make_hermitian(%s<imtime, matrix_valued> g)"%(gf_type, gf_view_type),
                doc = "Symmetrize the Green function in imaginary time, to ensure its hermiticity (G_ij[tau] = G_ji[tau]*)")
    m.add_function("%s<imtime, tensor_valued<4>> make_hermitian(%s<imtime, tensor_valued<4>> g)"%(gf_type, gf_view_type),
                doc = "Symmetrize the Green function in imaginary time, to ensure its hermiticity (G_ijkl[tau] = G_klij[tau]*)")

    # make_real_in_tau
    m.add_function("%s<imfreq, scalar_valued> make_real_in_tau(%s<imfreq, scalar_valued> g)"%(gf_type, gf_view_type),
                doc = "Symmetrize the Green function in freq, to ensure its real in tau (G[iw] = G[-iw]*)")
    m.add_function("%s<imfreq, matrix_valued> make_real_in_tau(%s<imfreq, matrix_valued> g)"%(gf_type, gf_view_type),
                doc = "Symmetrize the Green function in freq, to ensure its real in tau (G[iw](i,j) = G[-iw](i,j)*)")
    m.add_function("%s<imfreq, tensor_valued<4>> make_real_in_tau(%s<imfreq, tensor_valued<4>> g)"%(gf_type, gf_view_type),
                doc = "Symmetrize the Green function in freq, to ensure its real in tau (G[iw](i,j,k,l) = G[-iw](i,j,k,l)*)")

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

            if Meshes[1] in ["imtime", "retime"]:
                m.add_function(name = "make_gf_from_fourier",
                        signature="%s<%s, %s> make_gf_from_fourier(%s<%s, typename %s::real_t> g_in)"%(gf_type, Meshes[0], Target, gf_view_type, Meshes[1], Target),
                        doc ="""Create Green function from the Fourier transform of g_in""")

        # make_gf_from_fourier with known moments
        m.add_function(name = "make_gf_from_fourier",
               signature="gf<%s, %s> make_gf_from_fourier(gf_view<%s, %s> g_in, %s mesh, array_const_view<dcomplex, %s::rank + 1> known_moments)"%(Meshes[0], Target, Meshes[1], Target, Meshes[0], Target),
               doc ="""Create Green function from the Fourier transform of g_in using the known high-frequency moments""")

        # FIXME We are making copies of the tail
        m.add_function(name = "make_gf_from_fourier",
                signature="block_gf<%s, %s> make_gf_from_fourier(block_gf_view<%s, %s> g_in, %s mesh, std::vector<array<dcomplex, %s::rank + 1>> known_moments)"%(Meshes[0], Target, Meshes[1], Target, Meshes[0], Target),
               doc ="""Create Green function from the Fourier transform of g_in using the known high-frequency moments""")

        # FIXME We are making copies of the tail
        m.add_function(name = "make_gf_from_fourier",
                signature="block2_gf<%s, %s> make_gf_from_fourier(block2_gf_view<%s, %s> g_in, %s mesh, std::vector<std::vector<array<dcomplex, %s::rank + 1>>> known_moments)"%(Meshes[0], Target, Meshes[1], Target, Meshes[0], Target),
               doc ="""Create Green function from the Fourier transform of g_in using the known high-frequency moments""")

    # Additional overloads for make_gf_from_fourier
    m.add_function(name = "make_gf_from_fourier",
                   signature="gf<imfreq, %s> make_gf_from_fourier(gf_view<imtime, %s> g_in, int n_iw)"%(Target, Target),
                   doc ="""Create Green function from the Fourier transform of g_tau""")
    m.add_function(name = "make_gf_from_fourier",
                   signature="gf<imtime, %s> make_gf_from_fourier(gf_view<imfreq, %s> g_in, int n_tau)"%(Target, Target),
                   doc ="""Create Green function from the Fourier transform of g_iw""")
    m.add_function(name = "make_gf_from_fourier",
                   signature="block_gf<imfreq, %s> make_gf_from_fourier(block_gf_view<imtime, %s> g_in, int n_iw)"%(Target, Target),
                   doc ="""Create Green function from the Fourier transform of g_tau""")
    m.add_function(name = "make_gf_from_fourier",
                   signature="block_gf<imtime, %s> make_gf_from_fourier(block_gf_view<imfreq, %s> g_in, int n_tau)"%(Target, Target),
                   doc ="""Create Green function from the Fourier transform of g_iw""")

    m.add_function(name = "make_gf_from_fourier",
                   signature="gf<refreq, %s> make_gf_from_fourier(gf_view<retime, %s> g_in, bool shift_half_bin)"%(Target, Target),
                   doc ="""Create Green function from the Fourier transform of g_t""")
    m.add_function(name = "make_gf_from_fourier",
                   signature="gf<retime, %s> make_gf_from_fourier(gf_view<refreq, %s> g_in, bool shift_half_bin)"%(Target, Target),
                   doc ="""Create Green function from the Fourier transform of g_w""")

    # === Lattice Fourier
    for Meshes in [["cyclat", "brzone"], ["brzone", "cyclat"]]:
        for gf_type in ["gf", "block_gf", "block2_gf"]:
            gf_view_type = gf_type +  '_view'
            # make_gf_from_fourier
            m.add_function(name = "make_gf_from_fourier",
                    signature="%s<%s, %s> make_gf_from_fourier(%s<%s, %s> g_in)"%(gf_type, Meshes[0], Target, gf_view_type, Meshes[1], Target),
                    doc ="""Create Green function from the Fourier transform of g_in""")

# ---------------------- DLR mesh conversions --------------------
for Target in  ["scalar_valued", "matrix_valued", "tensor_valued<3>", "tensor_valued<4>"]:

    # dlr_imtime <-> dlr
    m.add_function(f"gf<dlr, {Target}> make_gf_dlr(gf_const_view<dlr_imtime, {Target}> g_tau)")
    m.add_function(f"gf<dlr_imtime, {Target}> make_gf_dlr_imtime(gf_const_view<dlr, {Target}> g_dlr)")

    # dlr_imfreq <-> dlr
    m.add_function(f"gf<dlr, {Target}> make_gf_dlr(gf_const_view<dlr_imfreq, {Target}> g_iw)")
    m.add_function(f"gf<dlr_imfreq, {Target}> make_gf_dlr_imfreq(gf_const_view<dlr, {Target}> g_dlr)")

    # imtime <-> dlr
    m.add_function(f"gf<dlr, {Target}> make_gf_dlr(gf_const_view<imtime, {Target}> g_tau, double w_max, double eps)")
    m.add_function(f"gf<imtime, {Target}> make_gf_imtime(gf_const_view<dlr, {Target}> g_dlr, long n_tau)")

    # imfreq <- dlr
    m.add_function(f"gf<imfreq, {Target}> make_gf_imfreq(gf_const_view<dlr, {Target}> g_dlr, long n_iw)")


########################
##   Code generation
########################

m.generate_code()
