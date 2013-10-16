cdef extern from "triqs/gfs/refreq.hpp" namespace "triqs::gfs" : 
  
    cdef cppclass refreq_domain :
        refreq_domain()

    cdef cppclass mesh_refreq "triqs::gfs::gf_mesh<triqs::gfs::refreq>"  :
        mesh_refreq ()
        mesh_refreq (mesh_refreq &)
        refreq_domain & domain()
        double x_min()
        double x_max()
        long size()
        double kind()
        bint operator ==( mesh_refreq &)

    cdef mesh_refreq make_mesh_refreq "triqs::gfs::gf_mesh<triqs::gfs::refreq>" (double omega_min, double omega_max, size_t n_freq, mesh_enum mk)
    #cdef mesh_refreq make_mesh_refreq "triqs::gfs::gf_factories<triqs::gfs::refreq>::make_mesh" (double omega_min, double omega_max, size_t n_freq, mesh_enum mk)

    cdef cppclass gf_refreq "triqs::python_tools::cython_proxy<triqs::gfs::gf_view<triqs::gfs::refreq>>" :
        gf_refreq()
        gf_refreq(gf_refreq &)
        # The constructor must be no_except, or the cython code won't be correct...
        gf_refreq(mesh_refreq, array_view[dcomplex, THREE], tail, nothing) #except +
        void operator << (gf_refreq &)
        mesh_refreq mesh() 
        array_view[dcomplex, THREE] data()
        tail singularity() 

cdef extern from "triqs/gfs/refreq.hpp"  :
    cdef void h5_write (h5_group, char *, gf_refreq &)

cdef extern from "triqs/utility/serialization.hpp"  :
    cdef std_string boost_serialize "triqs::serialize" (gf_refreq &) 
    cdef void boost_unserialize_into "triqs::deserialize_into_view" (std_string, gf_refreq &) 

# Python -> C
cdef gf_refreq as_gf_refreq (g) except +

# C -> Python 
cdef make_GfReFreq (gf_refreq x, indices_pack=*, name=*)

###############  Blocks of Im Time #########################

cdef extern from "triqs/gfs/block.hpp" namespace "triqs::gfs" : 

    cdef cppclass gf_block_refreq "triqs::python_tools::cython_proxy<triqs::gfs::gf_view<triqs::gfs::block_index,triqs::gfs::gf<triqs::gfs::refreq>>>" :
        gf_block_refreq()
        gf_refreq & operator [](int)
        discrete_mesh & mesh()

    cdef gf_block_refreq  make_gf_block_refreq "triqs::gfs::make_block_gf_view_from_vector<triqs::gfs::gf<triqs::gfs::refreq>>" (vector[gf_refreq] &) 
    #cdef gf_block_refreq  make_gf_block_refreq "triqs::gfs::make_gf_view<triqs::gfs::block_index,triqs::gfs::gf<triqs::gfs::refreq>>" (  vector[gf_refreq] &) 

cdef gf_block_refreq  as_gf_block_refreq (G) except +
cdef make_BlockGfReFreq (gf_block_refreq G, block_indices_pack=*, name=*)

