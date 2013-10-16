cdef extern from "triqs/gfs/imfreq.hpp" namespace "triqs::gfs" : 
  
    cdef cppclass imfreq_domain :
        double beta
        statistic_enum statistic
        imfreq_domain ()

    cdef cppclass mesh_imfreq "triqs::gfs::gf_mesh<triqs::gfs::imfreq>"  :
        mesh_imfreq ()
        mesh_imfreq (mesh_imfreq &)
        imfreq_domain & domain()
        double x_min()
        long size()
        bint operator ==( mesh_imfreq &)

    cdef mesh_imfreq make_mesh_imfreq "triqs::gfs::gf_mesh<triqs::gfs::imfreq>" (double beta, statistic_enum S, size_t n_max)
    #cdef mesh_imfreq make_mesh_imfreq "triqs::gfs::gf_implementation::gf_factories<triqs::gfs::imfreq,triqs::gfs::matrix>::make_mesh" (double beta, statistic_enum S, size_t n_max)
    
    cdef cppclass gf_imfreq "triqs::python_tools::cython_proxy<triqs::gfs::gf_view<triqs::gfs::imfreq> >" :
        gf_imfreq()
        gf_imfreq(gf_imfreq &)
        # The constructor must be no_except, or the cython code won't be correct...
        gf_imfreq(mesh_imfreq, array_view[dcomplex, THREE], tail, nothing) #except +
        void operator << (gf_imfreq &)
        mesh_imfreq mesh() 
        array_view[dcomplex, THREE] data()
        tail singularity() 

cdef extern from "triqs/gfs/imfreq.hpp"  :
    cdef void h5_write (h5_group, char *, gf_imfreq &)

cdef extern from "triqs/utility/serialization.hpp"  :
    cdef std_string boost_serialize "triqs::serialize" (gf_imfreq &) 
    cdef void boost_unserialize_into "triqs::deserialize_into_view" (std_string, gf_imfreq &) 

# Python -> C
cdef gf_imfreq  as_gf_imfreq (g) except +  

# C -> Python 
cdef make_GfImFreq (gf_imfreq x, indices_pack=*, name=*)

###############  Blocks of Im Freq #########################

cdef extern from "triqs/gfs/block.hpp" namespace "triqs::gfs" : 

    cdef cppclass gf_block_imfreq "triqs::python_tools::cython_proxy<triqs::gfs::gf_view<triqs::gfs::block_index,triqs::gfs::gf<triqs::gfs::imfreq>>>" :
        gf_block_imfreq()
        gf_imfreq & operator [](int)
        discrete_mesh & mesh()

    cdef gf_block_imfreq  make_gf_block_imfreq "triqs::gfs::make_block_gf_view_from_vector<triqs::gfs::gf<triqs::gfs::imfreq>>" (vector[gf_imfreq] &) 
    #cdef gf_block_imfreq  make_gf_block_imfreq "triqs::gfs::make_gf_view<triqs::gfs::block_index,triqs::gfs::gf<triqs::gfs::imfreq>>" (  vector[gf_imfreq] &) 

cdef gf_block_imfreq  as_gf_block_imfreq (G) except +
cdef make_BlockGfImFreq (gf_block_imfreq G, block_indices_pack=*, name=*)

