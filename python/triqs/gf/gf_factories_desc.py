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
for Target in  ["scalar_valued", "tensor_valued<1>", "matrix_valued", "tensor_valued<3>", "tensor_valued<4>"]:

    # === Matsubara and ReTime/Freq Fourier
    for gf_type in ["gf", "block_gf", "block2_gf"]:
        gf_view_type = gf_type +  '_view'

        for M0, M1 in [["imtime", "imfreq"], ["imfreq", "imtime"], ["retime", "refreq"], ["refreq", "retime"]]:
            # -- make_gf_from_fourier
            m.add_function(name = "make_gf_from_fourier",
                    signature="%s<%s, %s> make_gf_from_fourier(%s<%s, %s> g_in)"%(gf_type, M0, Target, gf_view_type, M1, Target),
                    doc ="""Create Green function from the Fourier transform of g_in""")

            if M1 in ["imtime", "retime"]:
                m.add_function(name = "make_gf_from_fourier",
                        signature="%s<%s, %s> make_gf_from_fourier(%s<%s, typename %s::real_t> g_in)"%(gf_type, M0, Target, gf_view_type, M1, Target),
                        doc ="""Create Green function from the Fourier transform of g_in""")

        # Mesh specific overloads of make_gf_from_fourier
        m.add_function(name = "make_gf_from_fourier",
                       signature="%s<imfreq, %s> make_gf_from_fourier(%s<imtime, %s> g_in, int n_iw)"%(gf_type, Target, gf_view_type, Target),
                       doc ="""Create Green function from the Fourier transform of g_tau""")
        m.add_function(name = "make_gf_from_fourier",
                       signature="%s<imfreq, %s> make_gf_from_fourier(%s<imtime, typename %s::real_t> g_in, int n_iw)"%(gf_type, Target, gf_view_type, Target),
                       doc ="""Create Green function from the Fourier transform of g_tau""")
        m.add_function(name = "make_gf_from_fourier",
                       signature="%s<imtime, %s> make_gf_from_fourier(%s<imfreq, %s> g_in, int n_tau)"%(gf_type, Target, gf_view_type, Target),
                       doc ="""Create Green function from the Fourier transform of g_iw""")

        m.add_function(name = "make_gf_from_fourier",
                       signature="%s<refreq, %s> make_gf_from_fourier(%s<retime, %s> g_in, bool shift_half_bin)"%(gf_type, Target, gf_view_type, Target),
                       doc ="""Create Green function from the Fourier transform of g_t""")
        m.add_function(name = "make_gf_from_fourier",
                       signature="%s<refreq, %s> make_gf_from_fourier(%s<retime, typename %s::real_t> g_in, bool shift_half_bin)"%(gf_type, Target, gf_view_type, Target),
                       doc ="""Create Green function from the Fourier transform of g_t""")
        m.add_function(name = "make_gf_from_fourier",
                       signature="%s<retime, %s> make_gf_from_fourier(%s<refreq, %s> g_in, bool shift_half_bin)"%(gf_type, Target, gf_view_type, Target),
                   doc ="""Create Green function from the Fourier transform of g_w""")

    for M0, M1 in [["imtime", "imfreq"], ["imfreq", "imtime"], ["retime", "refreq"], ["refreq", "retime"]]:

        # make_gf_from_fourier with known moments
        m.add_function(name = "make_gf_from_fourier",
               signature="gf<%s, %s> make_gf_from_fourier(gf_view<%s, %s> g_in, %s mesh, array_const_view<dcomplex, %s::rank + 1> known_moments)"%(M0, Target, M1, Target, M0, Target),
               doc ="""Create Green function from the Fourier transform of g_in using the known high-frequency moments""")

        # FIXME We are making copies of the tail
        m.add_function(name = "make_gf_from_fourier",
                signature="block_gf<%s, %s> make_gf_from_fourier(block_gf_view<%s, %s> g_in, %s mesh, std::vector<array<dcomplex, %s::rank + 1>> known_moments)"%(M0, Target, M1, Target, M0, Target),
               doc ="""Create Green function from the Fourier transform of g_in using the known high-frequency moments""")

        # FIXME We are making copies of the tail
        m.add_function(name = "make_gf_from_fourier",
                signature="block2_gf<%s, %s> make_gf_from_fourier(block2_gf_view<%s, %s> g_in, %s mesh, std::vector<std::vector<array<dcomplex, %s::rank + 1>>> known_moments)"%(M0, Target, M1, Target, M0, Target),
               doc ="""Create Green function from the Fourier transform of g_in using the known high-frequency moments""")

        if M1 in ["imtime", "retime"]:
            # make_gf_from_fourier with known moments
            m.add_function(name = "make_gf_from_fourier",
                           signature="gf<%s, %s> make_gf_from_fourier(gf_view<%s, typename %s::real_t> g_in, %s mesh, array_const_view<dcomplex, %s::rank + 1> known_moments)"%(M0, Target, M1, Target, M0, Target),
                   doc ="""Create Green function from the Fourier transform of g_in using the known high-frequency moments""")

            # FIXME We are making copies of the tail
            m.add_function(name = "make_gf_from_fourier",
                           signature="block_gf<%s, %s> make_gf_from_fourier(block_gf_view<%s, typename %s::real_t> g_in, %s mesh, std::vector<array<dcomplex, %s::rank + 1>> known_moments)"%(M0, Target, M1, Target, M0, Target),
                   doc ="""Create Green function from the Fourier transform of g_in using the known high-frequency moments""")

            # FIXME We are making copies of the tail
            m.add_function(name = "make_gf_from_fourier",
                           signature="block2_gf<%s, %s> make_gf_from_fourier(block2_gf_view<%s, typename %s::real_t> g_in, %s mesh, std::vector<std::vector<array<dcomplex, %s::rank + 1>>> known_moments)"%(M0, Target, M1, Target, M0, Target),
                   doc ="""Create Green function from the Fourier transform of g_in using the known high-frequency moments""")

    # === Lattice Fourier
    for M0, M1 in [["cyclat", "brzone"], ["brzone", "cyclat"]]:
        for gf_type in ["gf", "block_gf", "block2_gf"]:
            gf_view_type = gf_type +  '_view'
            # make_gf_from_fourier
            m.add_function(name = "make_gf_from_fourier",
                    signature="%s<%s, %s> make_gf_from_fourier(%s<%s, %s> g_in)"%(gf_type, M0, Target, gf_view_type, M1, Target),
                    doc ="""Create Green function from the Fourier transform of g_in""")

# ---------------------- DLR mesh conversions --------------------
for Target in  ["scalar_valued", "tensor_valued<1>", "matrix_valued", "tensor_valued<3>", "tensor_valued<4>"]:
    for gf_type in ["gf", "block_gf", "block2_gf"]:
        gf_view_type = gf_type +  '_const_view'

        # dlr_imtime <-> dlr
        m.add_function(f"{gf_type}<dlr, {Target}> make_gf_dlr({gf_view_type}<dlr_imtime, {Target}> g_tau)", doc="""Transform a DLR imaginary time or Matsubara Green's function to it's DLR coefficient representation""")
        m.add_function(f"{gf_type}<dlr_imtime, {Target}> make_gf_dlr_imtime({gf_view_type}<dlr, {Target}> g_dlr)", doc="""Transform a DLR coefficient Green's function to it's DLR imaginary time representation""")

        # dlr_imfreq <-> dlr
        m.add_function(f"{gf_type}<dlr, {Target}> make_gf_dlr({gf_view_type}<dlr_imfreq, {Target}> g_iw)", doc="""Transform a DLR imaginary time or Matsubara Green's function to it's DLR coefficient representation""")
        m.add_function(f"{gf_type}<dlr_imfreq, {Target}> make_gf_dlr_imfreq({gf_view_type}<dlr, {Target}> g_dlr)", doc="""Transform a DLR coefficient Green's function to it's DLR Matsubara frequency representation""")
    
        # dlr_imfreq <-> dlr (2D)
        m.add_function(f"{gf_type}<prod<dlr, dlr>, {Target}> make_gf_dlr({gf_view_type}<prod<dlr_imfreq, dlr_imfreq>, {Target}> g_iw)", calling_pattern = "auto result = make_gf_dlr<0, 1>(g_iw)", doc="""Transform a DLR imaginary time or Matsubara Green's function to it's DLR coefficient representation""")
        m.add_function(f"{gf_type}<prod<dlr_imfreq, dlr_imfreq>, {Target}> make_gf_dlr_imfreq({gf_view_type}<prod<dlr, dlr>, {Target}> g_dlr)", calling_pattern = "auto result = make_gf_dlr_imfreq<0, 1>(g_dlr)", doc="""Transform a DLR coefficient Green's function to it's DLR Matsubara frequency representation""")

        # dlr_imfreq <-> dlr (3D)
        m.add_function(f"{gf_type}<prod<dlr, dlr, dlr>, {Target}> make_gf_dlr({gf_view_type}<prod<dlr_imfreq, dlr_imfreq, dlr_imfreq>, {Target}> g_iw)", calling_pattern = "auto result = make_gf_dlr<0, 1, 2>(g_iw)", doc="""Transform a DLR imaginary time or Matsubara Green's function to it's DLR coefficient representation""")
        m.add_function(f"{gf_type}<prod<dlr_imfreq, dlr_imfreq, dlr_imfreq>, {Target}> make_gf_dlr_imfreq({gf_view_type}<prod<dlr, dlr, dlr>, {Target}> g_dlr)", calling_pattern = "auto result = make_gf_dlr_imfreq<0, 1, 2>(g_dlr)", doc="""Transform a DLR coefficient Green's function to it's DLR Matsubara frequency representation""")

        # imtime <-> dlr
        m.add_function(f"{gf_type}<dlr, {Target}> fit_gf_dlr({gf_view_type}<imtime, {Target}> g_tau, double w_max, double eps)", doc="""Perform a least square fit of a imaginary time Green's function to obtain a DLR coefficient representation""")
        m.add_function(f"{gf_type}<imtime, {Target}> make_gf_imtime({gf_view_type}<dlr, {Target}> g_dlr, long n_tau)", doc="""Transform a DLR coefficient Green's function to a imaginary time Green's function""")

        # imfreq <- dlr
        m.add_function(f"{gf_type}<imfreq, {Target}> make_gf_imfreq({gf_view_type}<dlr, {Target}> g_dlr, long n_iw)", doc="""Transform a DLR coefficient Green's function to a Matsubara frequency Green's function""")

# DLR Product Mesh Conversion
# Limit combinations to avoid compile-time blowup
for Target in  ["scalar_valued", "matrix_valued"]:
    for gf_type in ["gf", "block_gf"]:
        gf_view_type = gf_type +  '_const_view'
        for M in ['brzone', 'cyclat']:

            # dlr_imtime <-> dlr
            m.add_function(f"{gf_type}<prod<dlr, {M}>, {Target}> make_gf_dlr({gf_view_type}<prod<dlr_imtime, {M}>, {Target}> g_tau)", calling_pattern = "auto result = make_gf_dlr<0>(g_tau)" )
            m.add_function(f"{gf_type}<prod<{M}, dlr>, {Target}> make_gf_dlr({gf_view_type}<prod<{M}, dlr_imtime>, {Target}> g_tau)", calling_pattern = "auto result = make_gf_dlr<1>(g_tau)")
            m.add_function(f"{gf_type}<prod<dlr_imtime, {M}>, {Target}> make_gf_dlr_imtime({gf_view_type}<prod<dlr, {M}>, {Target}> g_dlr)", calling_pattern = "auto result = make_gf_dlr_imtime<0>(g_dlr)")
            m.add_function(f"{gf_type}<prod<{M}, dlr_imtime>, {Target}> make_gf_dlr_imtime({gf_view_type}<prod<{M}, dlr>, {Target}> g_dlr)", calling_pattern = "auto result = make_gf_dlr_imtime<1>(g_dlr)")

            # dlr_imfreq <-> dlr
            m.add_function(f"{gf_type}<prod<dlr, {M}>, {Target}> make_gf_dlr({gf_view_type}<prod<dlr_imfreq, {M}>, {Target}> g_iw)", calling_pattern = "auto result = make_gf_dlr<0>(g_iw)")
            m.add_function(f"{gf_type}<prod<{M}, dlr>, {Target}> make_gf_dlr({gf_view_type}<prod<{M}, dlr_imfreq>, {Target}> g_iw)", calling_pattern = "auto result = make_gf_dlr<1>(g_iw)")
            m.add_function(f"{gf_type}<prod<dlr_imfreq, {M}>, {Target}> make_gf_dlr_imfreq({gf_view_type}<prod<dlr, {M}>, {Target}> g_dlr)", calling_pattern = "auto result = make_gf_dlr_imfreq<0>(g_dlr)")
            m.add_function(f"{gf_type}<prod<{M}, dlr_imfreq>, {Target}> make_gf_dlr_imfreq({gf_view_type}<prod<{M}, dlr>, {Target}> g_dlr)", calling_pattern = "auto result = make_gf_dlr_imfreq<1>(g_dlr)")

            # imtime <-> dlr
            m.add_function(f"{gf_type}<prod<dlr, {M}>, {Target}> fit_gf_dlr({gf_view_type}<prod<imtime, {M}>, {Target}> g_tau, double w_max, double eps)", calling_pattern = "auto result = fit_gf_dlr<0>(g_tau, w_max, eps)")
            m.add_function(f"{gf_type}<prod<{M}, dlr>, {Target}> fit_gf_dlr({gf_view_type}<prod<{M}, imtime>, {Target}> g_tau, double w_max, double eps)", calling_pattern = "auto result = fit_gf_dlr<1>(g_tau, w_max, eps)")
            m.add_function(f"{gf_type}<prod<imtime, {M}>, {Target}> make_gf_imtime({gf_view_type}<prod<dlr, {M}>, {Target}> g_dlr, long n_tau)", calling_pattern = "auto result = make_gf_imtime<0>(g_dlr, n_tau)")
            m.add_function(f"{gf_type}<prod<{M}, imtime>, {Target}> make_gf_imtime({gf_view_type}<prod<{M}, dlr>, {Target}> g_dlr, long n_tau)", calling_pattern = "auto result = make_gf_imtime<1>(g_dlr, n_tau)")

            # imfreq <- dlr
            m.add_function(f"{gf_type}<prod<imfreq, {M}>, {Target}> make_gf_imfreq({gf_view_type}<prod<dlr, {M}>, {Target}> g_dlr, long n_iw)", calling_pattern = "auto result = make_gf_imfreq<0>(g_dlr, n_iw)")
            m.add_function(f"{gf_type}<prod<{M}, imfreq>, {Target}> make_gf_imfreq({gf_view_type}<prod<{M}, dlr>, {Target}> g_dlr, long n_iw)", calling_pattern = "auto result = make_gf_imfreq<1>(g_dlr, n_iw)")

########################
##   Code generation
########################

m.generate_code()
